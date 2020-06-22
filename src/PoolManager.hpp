// AxoMem Nucleus - for developing Persistent Applications
// Copyright (C) 2018-2020 AxoMem Pte Ltd.

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

#include <typeinfo>
#include <filesystem>
#include "Platform.hpp"


namespace nucleus {

/// @brief manages Persistent Memory Pools
template <class N>
class PoolManager {

public:
    explicit PoolManager(const std::string &fileName_arg, const std::string &layout_arg, const size_t pool_main_size) :
            file_name(fileName_arg) {

        Logging::log()->debug("PoolManager::() with pool file name {} and layout {}", file_name, layout_arg);

        if (std::filesystem::exists(std::filesystem::path(file_name))) {
            if (pmem::obj::pool<N>::check(file_name, layout_arg) == 1) {

                Logging::log()->info("Opening existing pool {} with layout {}", file_name, layout_arg);
                my_pool = pmem::obj::pool<N>::open(file_name, layout_arg);

            } else {

                throw pmem::pool_error(fmt::format("PMem pool file check failed during open."
                                                       "Check label matches {} via pmempool info.", layout_arg));

            }

        } else {

                Logging::log()->info("Creating new pool {} with layout '{}' and size {}", file_name, layout_arg,
                                     pool_main_size);
                my_pool = pmem::obj::pool<N>::create(file_name, layout_arg, pool_main_size);

                Logging::log()->info("Pool successfully created.");
                Logging::log()->warn("Please remember this is an Alpha version of Nucleus");

        }

    };
    ~PoolManager() {
        Logging::log()->info("Closing persistent memory pool for file {}", file_name);
        my_pool.close();
    };

    PoolManager(const PoolManager&)                = delete; // Copy
    PoolManager(PoolManager&&)                     = delete; // Move
    PoolManager& operator= ( const PoolManager & ) = delete; // Copy Assign
    PoolManager& operator= ( PoolManager && )      = delete; // Move assign

    pmem::obj::pool<N> &pool() {
        return my_pool;
    };

private:
    pmem::obj::pool<N> my_pool;
    const std::string &file_name;

};
}

#endif //NUCLEUS_POOL_H
