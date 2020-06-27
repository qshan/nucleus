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

        ctx->log->trace("AppManager is Preparing Rest Routes");

        ctx->rest_server->RegisterRoute(restinio::http_method_get(), "/api/v1/ping",
                          [this](const req_t &req, const params_t &params, resp_t &resp){

            resp["data"]["status"] = GetAppStateName();
            return restinio::status_ok();

        });

        ctx->log->trace("AppManager is calling app->Start()");

        app->Start(ctx);

        ctx->log->trace("AppManager is starting ReST Server. Disable ReST {}",
                         Config::to_string(config->rest_disable));

        ctx->rest_server->Start();

        SetAppState(nucleus::RUNNING);

        ctx->log->info("*** Server is running for {}", ctx->config->app_name);
        ctx->log->info("*** Press CTRL-C once to shutdown. "
                            "May require up to 3 presses for abnormal termination");

        // *** This Loop holds overall run state on main thread **************************
        while (GetAppState() == nucleus::RUNNING){

            if (!ctx->rest_server->last_error().empty()) {
                Exit(fmt::format("ReST Server has experienced an error: {}. Exiting",
                                 ctx->rest_server->last_error()));
            }

            CheckConditionPathExists(); // File based run lock

            std::this_thread::sleep_for(std::chrono::milliseconds{1000});

        }

        ctx->log->debug("AppManager Run Loop is exiting with AppState {}", GetAppStateName());

        ctx->log->debug("Closing ReST Server");
        ctx->rest_server->Stop(); // Reset unique ptr will close the server

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

        ctx->log->trace("App State is being set to {} (previous state {})", GetAppStateName(state), GetAppStateName());

        pmem::obj::transaction::run(pool_manager.pool(), [&state, this] {
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

};


}

#endif //NUCLEUS_APPMANAGER_HPP
