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

using namespace nucleus;

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
        pool = PoolManager::getPoolManager();
        pmem::obj::persistent_ptr<AppManager> appManager = pool->getAppManager();
        appManager->Run();
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
