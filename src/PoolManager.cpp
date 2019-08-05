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

}

PoolManager::~PoolManager()
{
    Logging::log()->info("Closing persistent memory pool");
    poolRoot.close();

}

PoolManager &
PoolManager::getPoolManager() {
    static PoolManager manager(config::pool_main_file);  // Singleton
    return manager;
}

pmem::obj::pool<rootStruct> &
PoolManager::getPoolForTransaction()
{
    return poolRoot;
}

}