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

#ifndef NUCLEUS_HPP
#define NUCLEUS_HPP

#include "Platform.hpp"
#include "AppManager.hpp"

namespace nucleus {

template <class M>
class Nucleus {

    static AppManager<M> *p_app_manager ;
    static int signal_times;

public:

    Nucleus (int argc, char *argv[]) {

        // Load configuration manager first. Needed for log settings
        try {
            config::load_config(argc, argv);
        } catch (const std::exception &exc) {
            //std::cerr << "Error: " << exc.what() << std::endl;
            throw std::runtime_error(fmt::format("Error loading configuration: {}", exc.what()));
        }

        // Load Logging Manager
        Logging::init();

    }

    int Run()
    {
        int exitCode = EXIT_FAILURE;
        auto log = Logging::log();
        log->info("Nucleus is starting");

        try {

            auto app_manager = AppManager<M>();

            p_app_manager = &app_manager;

            set_signal_handlers(); // to handle CTRL-C and make sure we can close pool

            app_manager.Run();

            exitCode = EXIT_SUCCESS;

        } catch (const pmem::transaction_error &err) {
            log->critical("Exception: pmem Transaction Error: {}", err.what());
        } catch (const pmem::transaction_scope_error &tse) {
            log->critical("Exception: pmem Transaction Scope Error: {} ",tse.what());
        } catch (const pmem::pool_error &pe) {
            log->critical("Exception: pmem PoolManager Error: {}. Check pmem is mounted, space available, "
                          "permissions are set, layout is correct label",
                          pe.what());
        } catch (const std::logic_error &le) {
            log->critical("Exception: Std Logic Error: {}",le.what());
        } catch (const std::exception &exc) {
            log->critical("Exception: General: {}", exc.what());
        }

        log->info("Exiting Nucleus with exit status {}", exitCode);
        return exitCode;
    }

    void set_signal_handlers(){
        // Install CTRL-C handler to try exit gracefully.
        // TODO - install handlers for other signals, eg if running a daemon or windows service

    #ifndef _WIN32
        // TODO - store previous handler information and restore?
        struct sigaction custom_handler = {{process_signal}};
        sigemptyset(&custom_handler.sa_mask);
        custom_handler.sa_flags = 0;
        sigaction(SIGINT, &custom_handler, nullptr);
        sigaction(SIGTERM, &custom_handler, nullptr);
    #else
        if (!SetConsoleCtrlHandler( (PHANDLER_ROUTINE) win_ctrlc_handler, TRUE)) {
            Logging::log()->warn("WARNING: Could not set CTRL-C handler");
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

    static void process_signal (int s) {

        signal_times++;
        if (signal_times == 1) {
            try {
                p_app_manager->Exit(s);
            } catch (const std::exception &exc) {
                Logging::log()->critical("Exception while attempting to request AppManager Exit {}", exc.what());
                signal_times = 99;
            }
        }

        if (signal_times > 2) {
            Logging::log()->critical("Received multiple interrupts. Restoring default handler. Press once more to exit");
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
