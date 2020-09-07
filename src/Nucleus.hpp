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

#ifndef NUCLEUS_HPP
#define NUCLEUS_HPP

#include "Platform.hpp"
#include "AppManager.hpp"
#include "RestServer.hpp"

#include <sstream>
#include <filesystem>
#include <typeinfo>

namespace nucleus {

/**
 * Nucleus manages the overall configuration and execution of the environment.
 * It loads configuration, starts logging, constructs the `AppManager<T>` instance and runs it.
 * It also handles the Signal handling for responding to OS calls like CTRL-C (SIGINT) or daemon termination (SIGHALT)
 * ### Example
 * Automatic execution: `return Nucleus<MyApp>(argc, argv).Run();`
 * @tparam T An instance of `AppManager<MyApp>`
 */
template <class T>
class Nucleus {

    static AppManager<T> *p_app_manager ;
    static int signal_times;

public:

    /**
     * Start Nucleus using a pre-built instance of nucleus::config configure the instance.
     * @param ctx_ref a shared pointer of an existing ctx object. Must remain in scope for life of Nucleus
     */

    explicit Nucleus (CTX context) : ctx(context) {
        if (ctx->config == nullptr || ctx->log == nullptr || ctx->rest_server == nullptr) {
            throw std::invalid_argument("If using custom Context, it must be fully initialized");
        }
    }

    /**
     * Start Nucleus using argc and argv typically from `main()` to configure the instance.
     * @param argc Arg count
     * @param argv Arg string array
     */

    Nucleus (int argc, char *argv[]) : ctx(nullptr) {

        try {
            config = std::make_unique<Config>(argc, argv);
        } catch (const std::invalid_argument &exc) {
            configuration_error = exc.what();
        }

    }

    /**
     * Run nucleus and block the thread until exit
     * @return An OS exit code with 0 as successful completion and 1 otherwise.
     */
    int Run()
    {
        int exitCode = EXIT_FAILURE;

        if (!configuration_error.empty()) {
            // we exit here so we can exit cleanly, otherwise would be an exception from config error
            return Config::print_help(configuration_error);
        }

        // Build our own context if not provided
        if (ctx == nullptr) {
            try {
                auto config_ptr = config.get();
                logger = std::make_unique<Logging>(config.get());
                auto logging_ptr = logger.get();
                rest_server = std::make_unique<RestServer>(config_ptr, logging_ptr);
                RestServer *rest_server_ptr = rest_server.get();
                ctx_instance = std::unique_ptr<Context>(new Context{config_ptr, logging_ptr, logger->get_logger(), rest_server_ptr});
                Context *ctx_ptr = ctx_instance.get();
                ctx = ctx_ptr;
            } catch (const std::exception &exc) {
                std::cout << "Exception during context setup: " << exc.what() << std::endl;
                return 1;
            }
        }

        ctx->log->info("The Nucleus engine is starting for app {}", ctx->config->app_name);

        try {

            auto app_manager = AppManager<T>(ctx);

            p_app_manager = &app_manager;

            set_signal_handlers(); // to handle CTRL-C and make sure we can close pool

            app_manager.Run();

            exitCode = EXIT_SUCCESS;

        } catch (const pmem::transaction_error &err) {
            ctx->log->critical("Exception: pmem Transaction Error: {}", err.what());
        } catch (const pmem::transaction_scope_error &tse) {
            ctx->log->critical("Exception: pmem Transaction Scope Error: {} ",tse.what());
        } catch (const pmem::pool_error &pe) {
            ctx->log->critical("Exception: pmem PoolManager Error: {}", pe.what());
            ctx->log->info("Check pmem is mounted, space available, permissions are set, layout is correct label");

        } catch (const std::exception &exc) {
            ctx->log->critical("Exception caught by Nucleus: {}: {}", typeid(exc).name(), exc.what());
        }

        ctx->log->info("Exiting {} with exit status {}", ctx->config->app_name, exitCode);
        return exitCode;
    }

private:

    Context* ctx;

    std::string configuration_error;

    std::unique_ptr<Config> config;
    std::unique_ptr<Logging> logger;
    std::unique_ptr<RestServer> rest_server;
    std::unique_ptr<Context> ctx_instance;

    /// Install CTRL-C handler to try exit gracefully.
    void set_signal_handlers() const {

    #ifndef _WIN32
        // TODO - store previous handler information and restore?
        struct sigaction custom_handler = {{process_signal}};
        sigemptyset(&custom_handler.sa_mask);
        custom_handler.sa_flags = 0;
        sigaction(SIGINT, &custom_handler, nullptr);
        sigaction(SIGTERM, &custom_handler, nullptr);
    #else
        if (!SetConsoleCtrlHandler( (PHANDLER_ROUTINE) win_ctrlc_handler, TRUE)) {
            //Logging::log()->warn("WARNING: Could not set CTRL-C handler");
            std::cout << "WARNING: Could not set CTRL-C handler" << std::endl;
        }
    #endif

    }

    #ifdef _WIN32
    static BOOL WINAPI win_ctrlc_handler (DWORD signal) {
        if (signal != CTRL_C_EVENT && signal != CTRL_CLOSE_EVENT) return FALSE;
        process_signal(1);
        return TRUE;
    }
    #endif

    /// Process a received signal from the OS, like SIGINT or SIGHALT
    static void process_signal (int signal) {

        signal_times++;
        if (signal_times == 1) {
            try {
                p_app_manager->Exit(signal);
            } catch (const std::exception &exc) {
                std::cerr << "Exception while attempting to request AppManager Exit: " <<  exc.what() << std::endl;
                signal_times = 99;
            }
        }

        if (signal_times > 2) {
            std::cerr << "Received multiple interrupts. Restoring default handler. Press once more to exit" << std::endl;
            #ifndef _WIN32
                struct sigaction default_action = {{SIG_DFL}};
                sigaction(SIGINT, &default_action, nullptr);
                sigaction(SIGTERM, &default_action, nullptr);
            #else
                SetConsoleCtrlHandler(NULL, FALSE);
            #endif
        }

    };

};

// These initialise static members for the templated class

template< class M >
int Nucleus<M>::signal_times = 0;

template< class M >
AppManager<M> *Nucleus<M>::p_app_manager = nullptr;

}
#endif //NUCLEUS_HPP
