//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <thread>
#include "PoolManager.hpp"
#include "Nucleus.hpp"

#define ASIO_HEADER_ONLY 1
#define ASIO_STANDALONE 1
#include <asio.hpp>
#include "AppManager.hpp"

using namespace nucleus;

AppManager *AppManager_g = nullptr; // Pointer to class instance
int signal_times = 0;

int
nucleus::NucleusRun(int argc, char *argv[])
{
    int exitCode = EXIT_FAILURE;

    // Load configuration manager first. Needed for log settings
    try {
        config::load_config(argc, argv);
    } catch (const std::exception &exc) {
        std::cerr << "Error: " <<  exc.what() << std::endl;
        return exitCode;
    }

    // Load Logging Manager
    Logging::init();
    auto log = Logging::log();

    log->info("Nucleus is starting");

    PoolManager *pool = nullptr;
    try {

        auto app_manager = AppManager();

        AppManager_g = & app_manager;  // needed for signal handler at present. Clean up when move Nucleus into class.
        set_signal_handlers(); // to handle CTRL-C and make sure we can close pool

        app_manager.Run();

    } catch (const pmem::transaction_error &err) {
        log->critical("Exception: pmem Transaction Error: {}", err.what());
    } catch (const pmem::transaction_scope_error &tse) {
        log->critical("Exception: pmem Transaction Scope Error: {} ",tse.what());
    } catch (const pmem::pool_error &pe) {
        log->critical("Exception: pmem PoolManager Error: {}. Check pmem is mounted, space available, permissions are set",
                   pe.what());
    } catch (const std::logic_error &le) {
        log->critical("Exception: Std Logic Error: {}",le.what());
    } catch (const std::exception &exc) {
        log->critical("Exception: General: {}", exc.what());
    }

    if (pool != nullptr) {
        log->debug("Attempting to close pool manager");
        delete pool;
        exitCode = EXIT_SUCCESS;
    }

    log->info("Exiting Nucleus with exit status {}", exitCode);
    return exitCode;
}

void
nucleus::set_signal_handlers(){
    // Install CTRL-C handler to try exit gracefully.
    // TODO - install handlers for other signals, eg if running a daemon or windows service

#ifndef _WIN32
    // TODO - store previous handler information and restore?
    struct sigaction custom_handler;
    custom_handler.sa_handler = process_signal; // static member function in this class
    sigemptyset(&custom_handler.sa_mask);
    custom_handler.sa_flags = 0;
    sigaction(SIGINT, &custom_handler, NULL);
#else
    if (!SetConsoleCtrlHandler( (PHANDLER_ROUTINE) win_ctrlc_handler, TRUE)) {
        Logging::log()->warn("WARNING: Could not set CTRL-C handler");
    }
#endif

};

#ifdef _WIN32
BOOL WINAPI nucleus::win_ctrlc_handler (DWORD signal) {
        if (signal != CTRL_C_EVENT && signal != CTRL_CLOSE_EVENT) return FALSE;
        process_signal(1);
        return TRUE;
    }
#endif

void
nucleus::process_signal (int s) {

    signal_times++;
    if (signal_times == 1) {
        try {
            AppManager_g->Exit(s);
        } catch (const std::exception &exc) {
            Logging::log()->critical("Exception while attempting to request AppManager Exit {}", exc.what());
            signal_times = 99;
        }
    }

    if (signal_times > 2) {
        Logging::log()->critical("Received multiple interrupts. Restoring default handler. Press once more to exit");
#ifndef _WIN32
        struct sigaction default_action;
        default_action.sa_handler = SIG_DFL;
        sigaction(SIGINT, &default_action, NULL);
#else
        SetConsoleCtrlHandler(NULL, FALSE);
#endif
    }

}