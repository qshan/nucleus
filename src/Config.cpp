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

// TODO - add further validation to provided values

namespace nucleus::config {

// declare globals
spdlog::level::level_enum log_level;
std::string log_file;
size_t pool_main_size;
std::string pool_main_file;

unsigned short rest_port;
std::string rest_address;
size_t rest_threads;

std::string condition_path;


bool
load_config(const std::string& executable_name_arg, int argc, char *argv[]) {

    // These are default settings.
    config::log_level = spdlog::level::debug;
    config::log_file = fmt::format("./{}.log", executable_name_arg);
    config::pool_main_size = (size_t) 1024*1024*1024*1; // 1GB
    config::pool_main_file = fmt::format("./{}.pmem", executable_name_arg);

    config::rest_port = 8080;
    config::rest_address = "localhost";
    config::rest_threads = 4;

    // condition_path is empty by default

    if (argc < 2) return false;

    auto string_args = args_to_string(argc, argv);

    if (ini_parse_string(string_args.c_str(), config::handler, nullptr) < 0) {
        throw std::invalid_argument("Unable to parse configuration data. Check config file or command line args");
    }

    return true;
}


int
handler(void* user, const char* section, const char* name, const char* value) {

    bool matched = false;

    auto check_match = [section, name, &matched](const std::string& section_arg, const std::string& name_arg) {
        // This is a closure-style sub function used below to simplify parsing
        auto found = ((section_arg == section) && (name_arg == name));
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

    if (check_match("","pool_main_size")) {
        config::pool_main_size = (size_t) std::stol(value) * 1024 * 1024;
        if (pool_main_size < 8 * 1024 * 1024) { // PMEMOBJ_MIN_POOL
            throw std::invalid_argument("Minimum pool size is 8MiB");
        }
    }

    if (check_match("","rest_port")) {
        auto temp_port = std::strtoul(value, nullptr,0);
        if (temp_port > USHRT_MAX || temp_port > 65535 || temp_port == 0) {
            throw std::invalid_argument("Given port is invalid. Should be 1 to 65535.");
        }
        config::rest_port = (unsigned short) temp_port;
    }
    if (check_match("","rest_address")) { config::rest_address = value;}
    if (check_match("","rest_threads")) { config::rest_threads = std::stoi(value);}

    if (check_match("","condition_path")) { config::condition_path = value; }

    if (!matched){
        std::string name_err = name;
        throw std::invalid_argument("Unknown configuration parameter specified in conf file or command line: " + name_err);
    }

    return matched;
}

std::string
args_to_string(int argc, char *argv[] ) {
    // convert args to a conf-file like string so we can process command line like file entries.
    // Note: command line overrides file
    std::string args;
    for (int i = 1; i < argc; i++) {
        std::string arg_part = std::regex_replace(argv[i], std::regex("^([-\\/]+)"), "");
        // check if the user asked for help
        if (arg_part[0] == 'h') {
            throw std::invalid_argument("HELP");
        }

        // Make sure the parameter has an = sign
        if (arg_part.find('=') == std::string::npos) {
            throw std::invalid_argument(fmt::format("Invalid command line parameter {}. "
                                                    "Format is --setting_name=value", arg_part));
        }

        // Add to the string.
        args += arg_part;
        args += "\r\n";
    }
    return args;
}


}