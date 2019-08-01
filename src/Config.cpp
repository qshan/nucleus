//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//
// TODO - keep track of start errors and add to logging once started
// TODO - change const char* to strings

#include <sstream>
#include <iostream>
#include <regex>
#include "Logging.hpp"
#include "ini.hpp"
#include "Config.hpp"
#include "spdlog/spdlog.h"

namespace config
{
// These should be default PROD settings.

    spdlog::level::level_enum log_level = spdlog::level::info;
    std::string log_file = "./nucleus.log";
    size_t pool_main_size = (size_t) 1024*1024*1024*1; // 1GB
    std::string pool_main_file = "./nucleus.pmem";

    int rest_port = 8080;
    int rest_threads = 4;
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
        throw std::logic_error("Unable to parse configuration data. Check config file or command line args");
    }

    return true;
}


int config::handler(void* user, const char* section, const char* name, const char* value) {

    bool matched = false;
    auto check_match = [section, name, &matched](std::string section_arg, std::string name_arg) -> bool {;
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
    if (check_match("","rest_threads")) { config::rest_threads = std::stoi(value);}

    if (!matched){
        std::string name_err = name;
        throw std::invalid_argument("Unknown configuration parameter specified in conf file or command line: " + name_err);
    }

    return matched;
}
