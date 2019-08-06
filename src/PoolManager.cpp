//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//
#include "PoolManager.hpp"

namespace nucleus {

PoolManager::PoolManager(const std::string &fileName)
{
    Logging::log()->debug("Creating PoolManager with root pool {}", fileName);

    if (pmem::obj::pool<NucleusPool>::check(fileName, LAYOUT_NAME) == 1) {

        Logging::log()->info("Opening existing pool {}", fileName);
        nucleus_pool = pmem::obj::pool<NucleusPool>::open(fileName, LAYOUT_NAME);

    } else {

        Logging::log()->info("Creating new pool {} with layout '{}' and size {}", fileName, LAYOUT_NAME, config::pool_main_size);
        nucleus_pool = pmem::obj::pool<NucleusPool>::create(
                fileName, LAYOUT_NAME, config::pool_main_size);
        Logging::log()->info("Pool successfully created.");
        Logging::log()->warn("Please remember Nucleus is alpha. Things *will* change and there *will* be bugs!");

    }

}

PoolManager::~PoolManager()
{
    Logging::log()->info("Closing persistent memory pool");
    nucleus_pool.close();

}

PoolManager &
PoolManager::getPoolManager() {

    static PoolManager manager(config::pool_main_file);  // Singleton
    return manager;

}

pmem::obj::pool<NucleusPool> &
PoolManager::getPoolForTransaction()
{
    return nucleus_pool;
}

}