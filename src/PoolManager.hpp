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

#include "Platform.hpp"
#define LAYOUT_NAME "myapp_v0.0.1"


namespace nucleus {

template <class N>
class PoolManager {

public:
    explicit PoolManager(const std::string &fileName_arg) : file_name(fileName_arg) {
        Logging::log()->debug("Creating PoolManager with pool file {}", file_name);

        if (pmem::obj::pool<N>::check(file_name, LAYOUT_NAME) == 1) {

            Logging::log()->info("Opening existing pool {}", file_name);
            my_pool = pmem::obj::pool<N>::open(file_name, LAYOUT_NAME);

        } else {

            Logging::log()->info("Creating new pool {} with layout '{}' and size {}", file_name, LAYOUT_NAME, config::pool_main_size);
            my_pool = pmem::obj::pool<N>::create(
                    file_name, LAYOUT_NAME, config::pool_main_size);
            Logging::log()->info("Pool successfully created.");
            Logging::log()->warn("Please remember Nucleus is alpha. Things *will* change and there *will* be bugs!");

        }

    };
    ~PoolManager() {
        Logging::log()->info("Closing persistent memory pool for file {}", file_name);
        my_pool.close();
    };
    pmem::obj::pool<N> &pool() {
        return my_pool;
    };

private:
    pmem::obj::pool<N> my_pool;
    const std::string &file_name;

};
}

#endif //NUCLEUS_POOL_H
