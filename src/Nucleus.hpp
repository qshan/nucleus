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
#include <sstream>
#include <filesystem>

namespace nucleus {

template <class M>
class Nucleus {

    static AppManager<M> *p_app_manager ;
    static int signal_times;

public:

    Nucleus (int argc, char *argv[]) {

        executable_name = std::filesystem::path(argv[0]).filename();

        // Load configuration manager first. Needed for log settings
        try {
            nucleus::config::load_config(executable_name, argc, argv);
        } catch (const std::invalid_argument &exc) {
            configuration_error = exc.what();
        }

    }

    int Run()
    {
        int exitCode = EXIT_FAILURE;

        if (!configuration_error.empty()) {
            return print_help();
        }

        // Load Logging Manager
        Logging::init();
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
            log->critical("Exception: pmem PoolManager Error: {}", pe.what());
            log->info("Check pmem is mounted, space available, permissions are set, layout is correct label");

        } catch (const std::exception &exc) {
            log->critical("Exception: General: {}", exc.what());
        }

        log->info("Exiting Nucleus with exit status {}", exitCode);
        return exitCode;
    }

private:

    std::string executable_name;
    std::string configuration_error;

    void set_signal_handlers() const {
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

    int print_help() const {

        std::stringstream usage;
        usage << std::endl << "Usage: " << executable_name << " [OPTIONS]" << std::endl
              << "Options:" << std::endl
              << "  -h --help                  This help information. See conf file for more details on arguments." << std::endl
              << "  --pool_main_file=filename  PMem pool path and name. Default is ./" << executable_name << ".pmem" << std::endl
              << "  --pool_main_size=1024      PMem pool initial size in MiB. No effect after first run" << std::endl
              << "  --log_file=filename        Log file path and name. Default is ./" << executable_name << ".log" << std::endl
              << "  --log_level=level          error, warn, info, debug, trace" << std::endl
              << "  --rest_address=localhost   ReST Server address in name or IP format (see conf for external access)" << std::endl
              << "  --rest_port=8080           ReST Server port number" << std::endl
              << "  --rest_threads=4           Number of threads for ReST server" << std::endl
              << "  --condition_path=filename  Server will exit if this specified path and file doesn't exist" << std::endl
              << std::endl;

        if (configuration_error.compare("HELP") != 0) {
            std::cerr << std::endl <<  configuration_error << std::endl;
            std::cerr << usage.str();
            return EXIT_FAILURE;
        } else {
            std::cout << usage.str();
            return EXIT_SUCCESS;  // since they asked for help
        }

    }
};

// These initialise static members for the templated class

template< class M >
int Nucleus<M>::signal_times = 0;

template< class M >
AppManager<M> *Nucleus<M>::p_app_manager = nullptr;

}
#endif //NUCLEUS_HPP
