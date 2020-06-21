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

#ifndef NUCLEUS_CONFIG_HPP
#define NUCLEUS_CONFIG_HPP

#include <string>
#include "Logging.hpp"

namespace nucleus::config {

bool load_config(const std::string& name, int argc, char *argv[]);

int handler(void* user, const char* section, const char* name, const char* value);
std::string args_to_string(int argc, char *argv[]);

// These are the config values to use.
// Also add to Config.cpp and nucleus.conf.template

// For logging
extern spdlog::level::level_enum log_level;
extern std::string log_file;

// For PoolManager - main pool file
extern size_t pool_main_size;
extern std::string pool_main_file;

// For ReSTServer
extern unsigned short rest_port;
extern std::string rest_address;
extern size_t rest_threads;

// For Testing or without ReST Server
extern std::string condition_path;

}

#endif //NUCLEUS_CONFIG_HPP
