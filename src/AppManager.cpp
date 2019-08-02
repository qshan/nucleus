//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//

#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <thread>
#include "AppManager.hpp"
#include "PoolManager.hpp"
#include "RestServer.hpp"

using namespace nucleus;
using namespace restinio;

AppManager::AppManager()
    : thingbase1{pmem::obj::make_persistent<MyApp>()}
    , appState{AppState::NEW}
{
    Logging::log()->trace("Constructed (persisting) AppManager");
}

AppManager::~AppManager()
{
    Logging::log()->trace("Deconstructing (deleting) AppManager");
    pmem::obj::transaction::run(
            PoolManager::getPoolManager()->getPoolForTransaction(), [&] {
                pmem::obj::delete_persistent<MyApp>(
                        thingbase1);
            });
}

void
AppManager::Run()
{
    Logging::log()->debug("AppManager is opening Application");
    Logging::log()->debug("Current App State is {} ", GetAppStateName());

    if (GetAppState() == nucleus::INITIALIZING) {
        throw std::runtime_error("AppManager: App failed initialization last time. Delete pool and try again");
    }

    if (GetAppState() == nucleus::NEW) {
        Logging::log()->info("AppManager: App initializing after first persistence");
        SetAppState(nucleus::INITIALIZING);
        thingbase1->Initialize();
    }

    if (GetAppState() != nucleus::INITIALIZING && GetAppState() != nucleus::STOPPED ) {
        Logging::log()->warn("AppManager: Abnormal App State detected. Attempting to restart however application state may be compromised");
    }

    Logging::log()->debug("AppManager is creating Rest server");
    auto restServer = RestServer::getRestServer(); // this triggers singleton

    SetAppState(nucleus::STARTING);
    thingbase1->Start();

    std::thread restserver_thread{ [&restServer] { restServer->run(); } };

    SetAppState(nucleus::RUNNING);
    Logging::log()->debug("AppManager Entering Main thread run loop with App State {}", GetAppStateName());

    std::cout << std::endl;
    std::cout << "***" << std::endl;
    std::cout << "Nucleus is running. Default site is http://localhost:" << config::rest_port << "/api/v1/ready" <<std::endl;
    std::cout << "Press CTRL-C once to shutdown normally. May require up to 3 presses in abnormal termination" << std::endl;
    std::cout << "***" << std::endl;
    std::cout << std::endl;

    while (appState == nucleus::RUNNING){
        // *** This holds overall run state **************************
        thingbase1->OnUpdate();
        std::this_thread::sleep_for(std::chrono::milliseconds{1000});
    }
    Logging::log()->debug("AppManager Run Loop is exiting with AppState {}", GetAppStateName());

    restServer->shutdown();
    restserver_thread.join();

    SetAppState(nucleus::STOPPED);
    Logging::log()->info("AppManager Run now exiting with AppState {}", GetAppStateName());

}

void
AppManager::SetAppState(AppState state)
{
    // TODO - basic state machine here to prevent incorrect state transitions, and make threadsafe
    pmem::obj::transaction::run(PoolManager::getPoolManager()->getPoolForTransaction(), [&] {
        Logging::log()->trace("App State is being set to {} (previous state {})", GetAppStateName(state), GetAppStateName());
        appState = state; });
}

nucleus::AppState
AppManager::GetAppState()
{
    return appState;
}

std::string
AppManager::GetAppStateName()
{
    return AppStateNames[appState];
}

std::string
AppManager::GetAppStateName(AppState state)
{
    return AppStateNames[state];
}

void
AppManager::resetApp()
{
    Logging::log()->info("App is now resetting");
    pmem::obj::transaction::run(
            PoolManager::getPoolManager()->getPoolForTransaction(), [&] {
                pmem::obj::delete_persistent<MyApp>(thingbase1);
                thingbase1 = pmem::obj::make_persistent<MyApp>();
            });
}

pmem::obj::persistent_ptr<MyApp>
AppManager::GetApp() {

    return thingbase1;

}

void
AppManager::Exit(const int s) {

    Logging::log()->debug("AppManager Exit CTRL SIGNAL Received: {}", s);
    Exit("SIGNAL Received");
}

void AppManager::Exit(const std::string &reason) {
    Logging::log()->info("AppManager Exit requested. Current App State is {}. Reason {}", GetAppStateName(), reason);
    SetAppState(nucleus::EXITING);
}
