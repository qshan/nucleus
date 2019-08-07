// AxoMem Nucleus - for developing Persistent Applications
// Copyright (C) 2018-2019 AxoMem Pte Ltd.

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License Version 2 (only)
// as published by the Free Software Foundation.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License Version 2 for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, see http://www.gnu.org/licenses/

#include <iostream>
#include <iomanip>
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
    Logging::log()->trace("Constructing AppManager");

    nucleus_pool = PoolManager::getPoolManager().nucleus_pool;
    if (nucleus_pool.root()->app == nullptr) {
        Logging::log()->debug("MyApp persistent object not yet initialized - persisting MyApp Object");
        pmem::obj::transaction::run(PoolManager::getPoolManager().getPoolForTransaction(), [&] {
            nucleus_pool.root()->app = pmem::obj::make_persistent<MyApp>();
            nucleus_pool.root()->app_state = AppState::NEW;
        });
    }

}

AppManager::~AppManager()
= default; // at present we don't support deleting app

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
        nucleus_pool.root()->app->Initialize();
    }

    if (GetAppState() != nucleus::INITIALIZING && GetAppState() != nucleus::STOPPED ) {
        Logging::log()->warn("AppManager: Abnormal App State detected. Attempting to restart however application state may be compromised");
    }

    Logging::log()->debug("AppManager is creating Rest server");

    SetAppState(nucleus::STARTING);
    nucleus_pool.root()->app->Start();

    RestServer rest_server {RestServerRouter::getRestServerRouter().getRouter()};

    SetAppState(nucleus::RUNNING);
    Logging::log()->debug("AppManager Entering Main thread run loop with App State {}", GetAppStateName());

    std::cout << std::endl;
    std::cout << "***" << std::endl;
    std::cout << "Nucleus is running. Default site is http://localhost:" << config::rest_port << "/api/v1/ready" <<std::endl;
    std::cout << "Press CTRL-C once to shutdown normally. May require up to 3 presses in abnormal termination" << std::endl;
    std::cout << "***" << std::endl;
    std::cout << std::endl;

    while (GetAppState() == nucleus::RUNNING){
        // *** This holds overall run state **************************
        std::this_thread::sleep_for(std::chrono::milliseconds{1000});
    }
    Logging::log()->debug("AppManager Run Loop is exiting with AppState {}", GetAppStateName());

    SetAppState(nucleus::STOPPED);
    Logging::log()->info("AppManager Run now exiting with AppState {}", GetAppStateName());

}

void
AppManager::SetAppState(AppState state)
{
    // TODO - basic state machine here to prevent incorrect state transitions, and make threadsafe
    pmem::obj::transaction::run(PoolManager::getPoolManager().getPoolForTransaction(), [&] {
        Logging::log()->trace("App State is being set to {} (previous state {})", GetAppStateName(state), GetAppStateName());
        nucleus_pool.root()->app_state = state; });
}

nucleus::AppState
AppManager::GetAppState()
{
    return nucleus_pool.root()->app_state;
}

std::string
AppManager::GetAppStateName()
{
    return AppStateNames[nucleus_pool.root()->app_state];
}

std::string
AppManager::GetAppStateName(AppState state)
{
    return AppStateNames[state];
}

pmem::obj::persistent_ptr<MyApp>
AppManager::GetApp() {

    return nucleus_pool.root()->app;
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
