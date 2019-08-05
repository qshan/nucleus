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
#include "json.hpp"

using namespace nucleus;
using namespace restinio;
using nlohmann::json;

AppManager::AppManager()
{
    Logging::log()->trace("Constructed (persisting) AppManager");

    pool_root = PoolManager::getPoolManager().poolRoot;
    if (pool_root.root()->thingbase1 == nullptr) {
        Logging::log()->debug("AppManager persistent object not yet initialized - persisting AppManager Object");
        pmem::obj::transaction::run(PoolManager::getPoolManager().getPoolForTransaction(), [&] {
            pool_root.root()->thingbase1 = pmem::obj::make_persistent<MyApp>();
            pool_root.root()->appState = AppState::NEW;
        });
    }

}

AppManager::~AppManager()
= default;

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
        pool_root.root()->thingbase1->Initialize();
    }

    if (GetAppState() != nucleus::INITIALIZING && GetAppState() != nucleus::STOPPED ) {
        Logging::log()->warn("AppManager: Abnormal App State detected. Attempting to restart however application state may be compromised");
    }

    Logging::log()->debug("AppManager is creating Rest server");

    SetAppState(nucleus::STARTING);
    pool_root.root()->thingbase1->Start();

    RestServer rest_server {RestServerRouter::getRestServerRouter().getRouter()};

    SetAppState(nucleus::RUNNING);
    Logging::log()->debug("AppManager Entering Main thread run loop with App State {}", GetAppStateName());

    std::cout << std::endl;
    std::cout << "***" << std::endl;
    std::cout << "Nucleus is running. Default site is http://localhost:" << config::rest_port << "/api/v1/ready" <<std::endl;
    std::cout << "Press CTRL-C once to shutdown normally. May require up to 3 presses in abnormal termination" << std::endl;
    std::cout << "***" << std::endl;
    std::cout << std::endl;

    auto my_app = pool_root.root()->thingbase1;
    while (GetAppState() == nucleus::RUNNING){
        // *** This holds overall run state **************************
        my_app->OnUpdate();
        std::this_thread::sleep_for(std::chrono::milliseconds{1000});
    }
    Logging::log()->debug("AppManager Run Loop is exiting with AppState {}", GetAppStateName());

    rest_server.shutdown();

    SetAppState(nucleus::STOPPED);
    Logging::log()->info("AppManager Run now exiting with AppState {}", GetAppStateName());

}

void
AppManager::SetAppState(AppState state)
{
    // TODO - basic state machine here to prevent incorrect state transitions, and make threadsafe
    pmem::obj::transaction::run(PoolManager::getPoolManager().getPoolForTransaction(), [&] {
        Logging::log()->trace("App State is being set to {} (previous state {})", GetAppStateName(state), GetAppStateName());
        pool_root.root()->appState = state; });
}

nucleus::AppState
AppManager::GetAppState()
{
    return pool_root.root()->appState;
}

std::string
AppManager::GetAppStateName()
{
    return AppStateNames[pool_root.root()->appState];
}

std::string
AppManager::GetAppStateName(AppState state)
{
    return AppStateNames[state];
}

pmem::obj::persistent_ptr<MyApp>
AppManager::GetApp() {

    return pool_root.root()->thingbase1;
}

void
AppManager::Exit(const int s) {

    Logging::log()->debug("AppManager Exit CTRL SIGNAL Received: {}", s);
    Exit("SIGNAL Received");
}

void
AppManager::Exit(const std::string &reason) {
    Logging::log()->info("AppManager Exit requested. Current App State is {}. Reason {}", GetAppStateName(), reason);
    SetAppState(nucleus::EXITING);
}
