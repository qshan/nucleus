// AxoMem Nucleus - for developing Persistent Applications
// Copyright (C) 2018-2020 AxoMem Pte Ltd.

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

// Improve - Implement a state machine to enforce state changes - https://github.com/axomem/nucleus/issues/48

#include <filesystem>
#include "PoolManager.hpp"
#include "RestServer.hpp"


namespace nucleus {

    #define APPSTATE_ENUM(APS) APS(NEW) APS(INITIALIZING) APS(STARTING) APS(RUNNING) APS(PAUSED) APS(EXITING) \
                           APS(STOPPED) APS(MAINTENANCE) APS(UNRECOVERABLE)
    MAKE_ENUM_AND_STRINGS(APPSTATE_ENUM, AppState, AppStateNames)

    // forward declaration for Pool Manager
    template <class N>
    class PoolManager;

    ///@brief Root structure of the pool file
    template <class M>
    class AppPool {
    public:
        pmem::obj::persistent_ptr<M> app;
        pmem::obj::p<AppState> app_state;
        ~AppPool()=delete;
    };

    ///@brief AppManager is the main container for the customer-developed app
    template <class A>
    class AppManager {

    public:

        AppManager() {
            Logging::log()->trace("Constructing AppManager");
            auto app_pool = pool_manager.pool();
            if (app_pool.root()->app == nullptr) {
                Logging::log()->debug("MyApp persistent object not yet initialized - persisting MyApp Object");
                pmem::obj::transaction::run(app_pool, [this, &app_pool] {
                    app_pool.root()->app = pmem::obj::make_persistent<A>();
                    app_pool.root()->app_state = AppState::NEW;
                });
            }

        };

        ~AppManager() {
            Logging::log()->trace("AppManager is exiting");
        };

        AppManager(const AppManager&)                = delete; // Copy
        AppManager(AppManager&&)                     = delete; // Move
        AppManager& operator= ( const AppManager & ) = delete; // Copy Assign
        AppManager& operator= ( AppManager && )      = delete; // Move assign


        void Run() {
            Logging::log()->debug("AppManager is opening Application. Current App State is {} ", GetAppStateName());

            auto app = pool_manager.pool().root()->app;

            if (GetAppState() == nucleus::INITIALIZING) {
                throw nucleus::app_error("AppManager: App failed initialization last time. "
                                         "Delete pool and try again");
            }

            if (GetAppState() == nucleus::NEW) {
                Logging::log()->debug("AppManager: App initializing after first persistence");
                SetAppState(nucleus::INITIALIZING);
                app->Initialize();
            }

            if (GetAppState() != nucleus::INITIALIZING && GetAppState() != nucleus::STOPPED ) {
                Logging::log()->warn("AppManager: Abnormal App State detected."
                                     "Starting App however application state may be compromised");
            }

            SetAppState(nucleus::STARTING);
            RegisterRestRoutes();
            app->Start();

            Logging::log()->trace("AppManager is creating ReST Server");
            std::unique_ptr<RestServer> rest_server(new RestServer(RestServerRouter::getRestServerRouter().getRouter(),
                                                                      config::rest_address, config::rest_port,
                                                                      config::rest_threads));

            SetAppState(nucleus::RUNNING);
            Logging::log()->debug("AppManager Entering Main thread run loop with App State {}", GetAppStateName());

            Logging::log()->info("***");
            Logging::log()->info("Server is running. Default site is "
                                 "http://{}:{}/api/v1/ready", config::rest_address, config::rest_port);
            Logging::log()->info("Press CTRL-C once to shutdown normally. May require up to 3 presses "
                                 "in abnormal termination");
            Logging::log()->info("***");

            while (GetAppState() == nucleus::RUNNING){
                // *** This holds overall run state **************************
                std::this_thread::sleep_for(std::chrono::milliseconds{1000});
                CheckConditionPathExists();
            }
            Logging::log()->debug("AppManager Run Loop is exiting with AppState {}", GetAppStateName());

            Logging::log()->debug("Closing Rest Server");
            rest_server.reset(nullptr); // Reset unique ptr will close the server

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

        PoolManager<AppPool<A>> pool_manager {config::pool_main_file, fmt::format("{}__v{}", typeid(A).name(),
                                              A::layout_version)};

        void SetAppState(AppState state) {

            pmem::obj::transaction::run(pool_manager.pool(), [&state, this] {
                Logging::log()->trace("App State is being set to {} (previous state {})",
                                      GetAppStateName(state), GetAppStateName());
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

        std::string GetAppStateName(AppState state) const
        {
            return AppStateNames[state];
        }

        void Exit(const std::string &reason) {
            Logging::log()->info("AppManager Exit requested. Current App State is {}. Reason {}",
                                 GetAppStateName(), reason);
            SetAppState(nucleus::EXITING);
        };

        void CheckConditionPathExists() {
            if (!config::condition_path.empty() && !std::filesystem::exists(config::condition_path)) {
                    Exit(fmt::format("Condition path is specified but does not or no longer exists."
                                     " Path is {}", config::condition_path));
            }
        }

        void RegisterRestRoutes() {
            auto router = RestServerRouter::getRestServerRouter().getRouter();

            router->http_get(
                    R"(/api/v1/ping)",
                    [this](auto req, auto params) {
                        nlohmann::json j = "{}"_json;
                        j["data"]["state"] = GetAppStateName();
                        j["response"]["message"] = "Ping command received";
                        return req->create_response()
                                .append_header( restinio::http_field_t::access_control_allow_origin, "*" )
                                .set_body( j.dump())
                                .done();
                    });


            // return the router to the RestServer
            RestServerRouter::getRestServerRouter().setRouter(std::move(router));

        }
    };


}

#endif //NUCLEUS_APPMANAGER_HPP
