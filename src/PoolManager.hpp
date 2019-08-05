//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//
#ifndef NUCLEUS_POOL_H
#define NUCLEUS_POOL_H

#include <string>
#include "Nucleus.hpp"

class MyApp;

namespace nucleus {
    class AppManager;

struct rootStruct {
    pmem::obj::persistent_ptr<MyApp> thingbase1;
    pmem::obj::p<AppState > appState;
};

class PoolManager {

    friend AppManager;

public:
    ~PoolManager();
    static PoolManager &getPoolManager();
    pmem::obj::pool<rootStruct> &getPoolForTransaction();

private:
    pmem::obj::pool<rootStruct> poolRoot;
    explicit PoolManager(const std::string &name);
    static PoolManager *poolManager;
    PoolManager(const PoolManager &);
    PoolManager &operator=(const PoolManager &);

};
}

#endif //NUCLEUS_POOL_H
