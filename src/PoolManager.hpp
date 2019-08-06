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
