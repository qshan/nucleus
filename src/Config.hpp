//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//

#ifndef NUCLEUS_CONFIG_HPP
#define NUCLEUS_CONFIG_HPP

#include <string>
#include "spdlog/spdlog.h"

namespace config
{
    bool load_config(int argc, char *argv[]);
    int handler(void* user, const char* section, const char* name, const char* value);

    // These are the config values to use.
    // Also add to Config.cpp and nucleus.conf.template

    // For logging
    extern spdlog::level::level_enum log_level;
    extern std::string log_file;

    // For PoolManager - main pool file
    extern size_t pool_main_size;
    extern std::string pool_main_file;

    // For ReSTServer
    extern int rest_port;
    extern int rest_threads;

}


#endif //NUCLEUS_CONFIG_HPP
