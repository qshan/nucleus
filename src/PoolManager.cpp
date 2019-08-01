//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//
#define LAYOUT_NAME "myapp_v0.0.1"
#include <thread>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <signal.h>
#include <stdio.h>
//#include <unistd.h> - windows testing - check still needed in linux? Think NULL not needed

#include "PoolManager.hpp"

namespace nucleus {


PoolManager *PoolManager::poolManager = nullptr; // Global pointer to PoolManager Singleton
int PoolManager::signal_times = 0;


PoolManager::PoolManager(const std::string &fileName)
{
    Logging::log()->debug("Creating PoolManager with root pool {}", fileName);
    if (pmem::obj::pool<rootStruct>::check(fileName, LAYOUT_NAME) == 1) {
        Logging::log()->info("Opening existing pool {}", fileName);
        poolRoot = pmem::obj::pool<rootStruct>::open(fileName, LAYOUT_NAME);
    } else {
        Logging::log()->info("Creating new pool {} with layout '{}' and size {}", fileName, LAYOUT_NAME, config::pool_main_size);
        poolRoot = pmem::obj::pool<rootStruct>::create(
                fileName, LAYOUT_NAME, config::pool_main_size);
        Logging::log()->info("Pool successfully created.");
        Logging::log()->warn("Please remember Nucleus is alpha. Things *will* change and there *will* be bugs!");
    }

    set_signal_handlers(); // to handle CTRL-C and make sure we can close pool

}

PoolManager::~PoolManager()
{
    Logging::log()->info("Closing persistent memory pool");
    poolRoot.close();

}

PoolManager *
PoolManager::getPoolManager()
{
    if (poolManager == nullptr) {
        Logging::log()->debug("PoolManager memory pointer not yet initialized - creating PoolManager object");
        poolManager = new PoolManager(config::pool_main_file);
    }
    return poolManager;
}

pmem::obj::persistent_ptr<AppManager>
PoolManager::getAppManager()
{

    assert(poolManager != nullptr && " Pool Manager null not expected here");

    if (poolRoot.root()->appManager == nullptr) {
            Logging::log()->debug("AppManager persistent object not yet initialized - persisting AppManager Object");
            pmem::obj::transaction::run(getPoolForTransaction(), [&] {
                poolRoot.root()->appManager = pmem::obj::make_persistent<AppManager>();
            });
    }

    return poolRoot.root()->appManager;
}

pmem::obj::pool<rootStruct> &
PoolManager::getPoolForTransaction()
{
    return poolRoot;
}

void
PoolManager::set_signal_handlers(){
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
    BOOL WINAPI PoolManager::win_ctrlc_handler (DWORD signal) {
        if (signal != CTRL_C_EVENT && signal != CTRL_CLOSE_EVENT) return FALSE;
        getPoolManager()->process_signal(1);
        return TRUE;
    }
#endif

void
PoolManager::process_signal (int s) {

    signal_times++;
    if (signal_times == 1) {
        try {
            getPoolManager()->getAppManager()->Exit(s);
        } catch (std::exception &exc) {
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
}