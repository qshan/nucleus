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
template <class T>
class PoolManager;

///@brief Root structure of the pool file
template <class T>
class AppPool {
public:
    pmem::obj::persistent_ptr<T> app;
    pmem::obj::p<AppState> app_state;
    ~AppPool()=delete;
};

///@brief AppManager is the main container for the customer-developed app
template <class T>
class AppManager {

template <class N>
friend class Nucleus;

public:

    explicit AppManager(const CTX& ctx_arg) : ctx(ctx_arg) {
        ctx->log->trace("Constructing AppManager for app {}", ctx->config->app_name);
        auto app_pool = pool_manager.pool();
        if (app_pool.root()->app == nullptr) {
            ctx->log->debug("App persistent object not yet initialized - persisting App Object");
            pmem::obj::transaction::run(app_pool, [this, &app_pool] {
                app_pool.root()->app = pmem::obj::make_persistent<T>(ctx);
                app_pool.root()->app_state = AppState::NEW;
            });
        }
    };

    ~AppManager() {
        ctx->log->trace("AppManager is exiting");
    };

    AppManager(const AppManager&)                = delete; // Copy
    AppManager(AppManager&&)                     = delete; // Move
    AppManager& operator= ( const AppManager & ) = delete; // Copy Assign
    AppManager& operator= ( AppManager && )      = delete; // Move assign

    /**
     * Runs the templated application.
     * This function blocks the thread until the the AppState is changed from RUNNING.
     */
    void Run() {
        auto config = ctx->config;

        ctx->log->info("AppManager is opening {}. Current App State is {} ", ctx->config->app_name, GetAppStateName());

        auto app = pool_manager.pool().root()->app;

        if (GetAppState() == nucleus::INITIALIZING) {
            throw exceptions::app_error("AppManager: App failed initialization last time. "
                                     "Delete pool and try again");
        }

        if (GetAppState() == nucleus::NEW) {
            ctx->log->debug("AppManager: App initializing after first persistence");
            SetAppState(nucleus::INITIALIZING);
            app->Initialize(ctx);
        }

        if (GetAppState() != nucleus::INITIALIZING && GetAppState() != nucleus::STOPPED ) {
            ctx->log->warn("AppManager: Abnormal App State detected."
                                 "Starting App however application state may be compromised");
        }

        SetAppState(nucleus::STARTING);

        app->Start(ctx);

        ctx->log->trace("AppManager is Preparing Rest Routes");

        auto rest_server_router = RestServerRouter(ctx);
        rest_server_router.setRouter(RegisterRestRoutes(app, std::move(rest_server_router.getRouter())));

        ctx->log->trace("AppManager is creating ReST Server. Disable ReST {}",
                         Config::to_string(config->rest_disable));

        std::unique_ptr<RestServer> rest_server(new RestServer(ctx, rest_server_router.getRouter(),
                                                                  config->rest_address, config->rest_port,
                                                                  config->rest_threads));

        SetAppState(nucleus::RUNNING);
        ctx->log->debug("AppManager Entering Main thread run loop with App State {}", GetAppStateName());

        ctx->log->info("***");
        ctx->log->info("Server is running for {}", ctx->config->app_name);
        ctx->log->info("Press CTRL-C once to shutdown. May require up to 3 presses for abnormal termination");
        ctx->log->info("Ping ReST URL is http://{}:{}/api/v1/ping", config->rest_address, config->rest_port);
        ctx->log->info("***");

        while (GetAppState() == nucleus::RUNNING){
            // *** This holds overall run state **************************

            std::this_thread::sleep_for(std::chrono::milliseconds{1000});

            CheckConditionPathExists();

            if (!rest_server->last_error().empty()) {
                // Redo this as check function when ReST server moved to ctx
                Exit(fmt::format("ReST Server has experienced an error: {}. Exiting",
                                 rest_server->last_error()));
            }

        }
        ctx->log->debug("AppManager Run Loop is exiting with AppState {}", GetAppStateName());

        ctx->log->debug("Closing ReST Server");
        rest_server.reset(nullptr); // Reset unique ptr will close the server

        app->Stop();
        SetAppState(nucleus::STOPPED);
        ctx->log->info("AppManager Run now exiting with AppState {}", GetAppStateName());

    }


private:

    std::shared_ptr<Context> ctx;

    PoolManager<AppPool<T>> pool_manager {ctx, ctx->config->pool_main_file,
                                          fmt::format("{}__v{}", typeid(T).name(), T::layout_version),
                                          ctx->config->pool_main_size};

    void SetAppState(AppState state) {

        pmem::obj::transaction::run(pool_manager.pool(), [&state, this] {
            ctx->log->trace("App State is being set to {} (previous state {})",
                                  GetAppStateName(state), GetAppStateName());
            pool_manager.pool().root()->app_state = state;
        });
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

    /**
     * Trigger an application closure from a OS Signal
     * * @param signal The signal id, eg SIGINT, SIGHALT etc
     */
    void Exit(int signal) {
        // for Signal types - move back to private after setting up Nucleus class def
        ctx->log->debug("AppManager Exit CTRL SIGNAL Received: {}", signal);
        Exit("SIGNAL Received");
    }

    void Exit(const std::string &reason) {
        ctx->log->info("AppManager Exit requested. Current App State is {}. Reason {}",
                             GetAppStateName(), reason);
        SetAppState(nucleus::EXITING);
    };

    void CheckConditionPathExists() {
        if (!ctx->config->condition_path.empty() && !std::filesystem::exists(ctx->config->condition_path)) {
                Exit(fmt::format("Condition path is specified but does not or no longer exists."
                                 " Path is {}", ctx->config->condition_path));
        }
    }

    std::unique_ptr<RestServerRouter::router_t> RegisterRestRoutes(pmem::obj::persistent_ptr<T> app, std::unique_ptr<RestServerRouter::router_t> router) {

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

        router = app->RegisterRestRoutes(std::move(router));

        return router;

    }
};


}

#endif //NUCLEUS_APPMANAGER_HPP
