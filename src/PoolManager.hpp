//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//
#ifndef NUCLEUS_POOL_H
#define NUCLEUS_POOL_H

#include <string>

#include "Nucleus.hpp"
#include "AppManager.hpp"

namespace nucleus {

struct rootStruct {
    pmem::obj::persistent_ptr<AppManager> appManager;
};

class PoolManager {

public:
    ~PoolManager();
    static PoolManager *getPoolManager();
    pmem::obj::persistent_ptr<AppManager> getAppManager();
    pmem::obj::pool<rootStruct> &getPoolForTransaction();
    static int signal_times;

private:
    pmem::obj::pool<rootStruct> poolRoot;
    explicit PoolManager(const std::string &name);
    static PoolManager *poolManager;
    void set_signal_handlers();
#ifdef _WIN32
    static BOOL WINAPI win_ctrlc_handler (DWORD signal);
#endif

    static void process_signal(int s);
    PoolManager(const PoolManager &);
    PoolManager &operator=(const PoolManager &);

};
}

#endif //NUCLEUS_POOL_H
