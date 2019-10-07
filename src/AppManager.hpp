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

#ifndef NUCLEUS_APPMANAGER_HPP
#define NUCLEUS_APPMANAGER_HPP

#include "PoolManager.hpp"
#include "RestServer.hpp"


namespace nucleus {

    #define APPSTATE_ENUM(APS) APS(NEW) APS(INITIALIZING) APS(STARTING) APS(RUNNING) APS(PAUSED) APS(EXITING) \
                           APS(STOPPED) APS(MAINTENANCE) APS(UNRECOVERABLE)
    MAKE_ENUM_AND_STRINGS(APPSTATE_ENUM, AppState, AppStateNames)

    template <class N>
    class PoolManager; // forward declaration

    template <class M>
    class AppPool {
        // This is the root structure of the pool file
    public:
        pmem::obj::persistent_ptr<M> app;
        pmem::obj::p<AppState> app_state;
    };

    template <class A>
    class AppManager {

    public:
        AppManager():
                pool_manager{PoolManager<AppPool<A>>(config::pool_main_file)}
        {
            Logging::log()->trace("Constructing AppManager");
            auto app_pool = pool_manager.pool();
            if (app_pool.root()->app == nullptr) {
                Logging::log()->debug("MyApp persistent object not yet initialized - persisting MyApp Object");
                pmem::obj::transaction::run(app_pool, [&] {
                    app_pool.root()->app = pmem::obj::make_persistent<A>();
                    app_pool.root()->app_state = AppState::NEW;
                });
            }

        };

        ~AppManager() {
            Logging::log()->trace("AppManager is exiting");
        };

        void Run() {
            Logging::log()->debug("AppManager is opening Application");
            Logging::log()->debug("Current App State is {} ", GetAppStateName());

            auto app = pool_manager.pool().root()->app;

            if (GetAppState() == nucleus::INITIALIZING) {
                throw std::runtime_error("AppManager: App failed initialization last time. Delete pool and try again");
            }

            if (GetAppState() == nucleus::NEW) {
                Logging::log()->info("AppManager: App initializing after first persistence");
                SetAppState(nucleus::INITIALIZING);
                app->Initialize();
            }

            if (GetAppState() != nucleus::INITIALIZING && GetAppState() != nucleus::STOPPED ) {
                Logging::log()->warn("AppManager: Abnormal App State detected. Attempting to restart however application state may be compromised");
            }

            SetAppState(nucleus::STARTING);
            app->Start();

            Logging::log()->debug("AppManager is creating ReST Server");
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
            app->Stop();
            SetAppState(nucleus::STOPPED);
            Logging::log()->info("AppManager Run now exiting with AppState {}", GetAppStateName());

        }

        void Exit(int s) {
            // for Signal types - move back to private after setting up Nucleus class def
            Logging::log()->debug("AppManager Exit CTRL SIGNAL Received: {}", s);
            Exit("SIGNAL Received");
        }

    private:
        PoolManager<AppPool<A>> pool_manager;

        void SetAppState(AppState state) {
            // TODO - basic state machine here to prevent incorrect state transitions, and make threadsafe
            pmem::obj::transaction::run(pool_manager.pool(), [&] {
                Logging::log()->trace("App State is being set to {} (previous state {})", GetAppStateName(state), GetAppStateName());
                pool_manager.pool().root()->app_state = state; });
        }

        AppState GetAppState()
        {
            return pool_manager.pool().root()->app_state;
        }

        std::string GetAppStateName()
        {
            return AppStateNames[pool_manager.pool().root()->app_state];
        }

        std::string GetAppStateName(AppState state)
        {
            return AppStateNames[state];
        }

        void Exit(const std::string &reason) {
            Logging::log()->info("AppManager Exit requested. Current App State is {}. Reason {}", GetAppStateName(), reason);
            SetAppState(nucleus::EXITING);
        };
    };


}

#endif //NUCLEUS_APPMANAGER_HPP
