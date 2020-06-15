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

#include <iostream>
#include <regex>
#include "ini.hpp"
#include "Config.hpp"
#include "Logging.hpp"

namespace nucleus {
namespace config
{
// These are default settings.

    spdlog::level::level_enum log_level = spdlog::level::debug;
    std::string log_file = "./nucleus.log";
    size_t pool_main_size = (size_t) 1024*1024*1024*1; // 1GB
    std::string pool_main_file = "./nucleus.pmem"; // note - replaced below if argv

    int rest_port = 8080;
    std::string rest_address = "localhost";
    size_t rest_threads = 4;

    std::string condition_path_exists;
}

bool config::load_config(int argc, char *argv[]) {

    if (argc < 2) return false;

    // convert args to a conf-file like string so we can process command line like file entries.
    // Note: command line overrides file
    std::string args;
    for (int i = 1; i < argc; i++) {
        args += argv[i];
        args += "\r\n";
    }

    if (ini_parse_string(args.c_str(), config::handler, nullptr) < 0) {
        throw std::invalid_argument("Unable to parse configuration data. Check config file or command line args");
    }

    return true;
}


int config::handler(void* user, const char* section, const char* name, const char* value) {

    bool matched = false;

    auto check_match = [section, name, &matched](const std::string& section_arg, const std::string& name_arg) {
        // This is a closure-style sub function used below to simplify parsing
        std::string name_part = std::regex_replace(name, std::regex("--"), "");
        auto found = (bool) ((section_arg == section) && (name_arg == name_part));
        if (found) {
            matched = true;
        }
        return found;
    };

    // If a config file has been specified, process that first
    if (check_match("","config_file")) {
        ini_parse(value, config::handler, nullptr);
    }

    if (check_match("","log_file")) { config::log_file = value; }
    if (check_match("","log_level")) { config::log_level = spdlog::level::from_str(value); }

    if (check_match("","pool_main_file")) { config::pool_main_file = value; }
    if (check_match("","pool_main_size")) { config::pool_main_size = (size_t) std::stol(value) * 1024 * 1024; }

    if (check_match("","rest_port")) { config::rest_port = std::stoi(value);}
    if (check_match("","rest_address")) { config::rest_address = value;}
    if (check_match("","rest_threads")) { config::rest_threads = std::stoi(value);}

    if (check_match("","condition_path_exists")) { config::condition_path_exists = value; }

    if (!matched){
        std::string name_err = name;
        throw std::invalid_argument("Unknown configuration parameter specified in conf file or command line: " + name_err);
    }

    return matched;
}
}