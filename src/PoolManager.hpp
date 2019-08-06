//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//
#ifndef NUCLEUS_POOL_H
#define NUCLEUS_POOL_H

#include "Nucleus.hpp"
#define LAYOUT_NAME "myapp_v0.0.1"

class MyApp;

namespace nucleus {

    class AppManager;

struct NucleusPool {
    // This is the root structure of the pool file
    pmem::obj::persistent_ptr<MyApp> app;
    pmem::obj::p<AppState > app_state;
};

class PoolManager {

    friend AppManager;

public:
    ~PoolManager();
    static PoolManager &getPoolManager();
    pmem::obj::pool<NucleusPool> &getPoolForTransaction();

private:
    pmem::obj::pool<NucleusPool> nucleus_pool;
    explicit PoolManager(const std::string &name);
    PoolManager(const PoolManager &);
    PoolManager &operator=(const PoolManager &);

};
}

#endif //NUCLEUS_POOL_H
