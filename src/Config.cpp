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
#include <fstream>
#include <regex>
#include <functional>
#include <filesystem>
#include "Config.hpp"

// TODO - add further validation to provided values

namespace nucleus {

Config::Config() {
    set_name("nucleus");
};

void Config::set_name(const std::string& name_arg) {

    if (name_arg.empty()) {
        throw std::invalid_argument("app_name cannot be empty");
    }
    app_name = name_arg;
    pool_main_file = fmt::format("./{}.pmem", app_name);
    log_file = fmt::format("./{}.log", app_name);

}

Config::Config(const std::string& name_arg)
{
    set_name(name_arg);
}

Config::Config(int argc, char *argv[]) {

    config_parse_args(argc, argv);
};

void
Config::config_parse_args(int argc, char *argv[]) {

    if (argc == 0) { throw std::invalid_argument("Args must have at least 1 arg - the executable name"); }

    set_name(std::filesystem::path(argv[0]).filename());

    auto string_args = args_to_stringstream(argc, argv);

    handler_t callback = [this](const std::string& section, const std::string& name, const std::string& value) {
        return handler(section, name, value); };

    ini_parse_stringstream(string_args, callback);

}

void
Config::handler(const std::string& section, const std::string& name, const std::string& value) {

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
        handler_t callback = [this](const std::string& section_arg, const std::string& name_arg,
                                    const std::string& value_arg) {
            return handler(section_arg, name_arg, value_arg); };
        ini_parse_file(value, callback);
    }

    if (check_match("","log_file")) { log_file = value; }

    if (check_match("","log_level")) { log_level = spdlog::level::from_str(value); }

    if (check_match("","pool_main_file")) { pool_main_file = value; }

    if (check_match("","pool_main_size")) {
        pool_main_size = (size_t) std::stol(value) * 1024 * 1024;
        if (pool_main_size < 8 * 1024 * 1024) { // PMEMOBJ_MIN_POOL
            throw std::invalid_argument("Minimum pool size is 8MiB");
        }
    }

    if (check_match("","rest_disable")) { rest_disable = parse_bool(value);}

    if (check_match("","rest_port")) { rest_port = parse_ip_port(value);}

    if (check_match("","rest_address")) { rest_address = value;}

    if (check_match("","rest_threads")) { rest_threads = std::stoi(value);}


    if (check_match("","condition_path")) { condition_path = value; }

    if (!matched){
        throw std::invalid_argument("Unknown configuration parameter specified in conf file or command line: " + name);
    }

}

std::stringstream
Config::args_to_stringstream(int argc, char *argv[] ) {

    // convert args to a conf-file like string so we can process command line like file entries.

    std::stringstream args;

    for (int i = 1; i < argc; i++) {  // we start at 1 as 0 is the exe name...

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
        args << arg_part << std::endl;

    }
    return args;
}

void
Config::ini_parse_stringstream(std::stringstream& stream, const handler_t& callback) {

    std::string line;

    while(std::getline(stream, line,'\n')){

        ltrim(line);
        rtrim(line);

        if (line.empty() || line.front() == '#' ) { // comment or empty
            continue;
        }

        auto delim_pos = line.find('=');
        if (delim_pos == std::string::npos) {
            throw std::invalid_argument("Parsing error in config: missing equals sign in parameter: " + line);
        }

        std::string variable = line.substr(0, delim_pos);
        rtrim(variable);

        std::string value = delim_pos + 1 < line.length() ? line.substr(delim_pos + 1, line.length()-delim_pos) : "";
        ltrim(value);

        callback("", variable, value);

    }

}

void
Config::ini_parse_file(const std::string& filename, const handler_t& callback) {

    std::stringstream buffer;

    std::ifstream file( filename );

    if (!file.is_open()) {
        throw std::invalid_argument("Cannot open file " + filename + " error: " + strerror(errno));
    }
    buffer << file.rdbuf();
    file.close();

    ini_parse_stringstream(buffer, callback);

}

unsigned short
Config::parse_ip_port(const std::string &port) {

    auto err_msg = "Given port is invalid. Should be 1 to 65535.";

    if (!std::regex_match(port, std::regex("[0-9]+"))) {
        throw std::invalid_argument(err_msg );
    }

    auto temp_port = std::stol(port, nullptr,0);

    if (temp_port > USHRT_MAX || temp_port > 65535 || temp_port == 0) {
        throw std::invalid_argument(err_msg);
    }

    return (unsigned short) temp_port;

}

bool
Config::parse_bool(const std::string &str_arg) {

    auto str = str_arg;

    std::transform(str.begin(), str.end(), str.begin(), ::toupper);

    if (str == "1" || str == "TRUE" || str == "YES") {
        return true;
    } else if (str == "0" || str == "FALSE" || str == "NO") {
        return false;
    } else {
        throw std::invalid_argument("Cannot parse '" + str_arg + "' to bool.");
    }

}

std::string
Config::to_string(bool bool_arg) {
    return bool_arg ? "true" : "false";
}

/// Print help message



int Config::print_help(const std::exception& config_exception) {
    return print_help((std::string) config_exception.what());
}

int Config::print_help(const std::string& configuration_error) {

    std::string help_app_name = "<appname>";

    std::stringstream usage;
    usage << std::endl << "Usage: " << help_app_name << " [OPTIONS]" << std::endl
          << "Available Options:" << std::endl
          << "  -h --help                  This help information. See conf file for more details on arguments." << std::endl
          << "  --pool_main_file=filename  PMem pool path and name. Default is ./" << help_app_name << ".pmem" << std::endl
          << "  --pool_main_size=1024      PMem pool initial size in MiB. No effect after first run" << std::endl
          << "  --log_file=filename        Log file path and name. Default is ./" << help_app_name << ".log" << std::endl
          << "  --log_level=level          error, warn, info, debug, trace" << std::endl
          << "  --rest_address=localhost   ReST Server address in name or IP format (see conf for external access)" << std::endl
          << "  --rest_port=8080           ReST Server port number" << std::endl
          << "  --rest_threads=4           Number of threads for ReST server" << std::endl
          << "  --condition_path=filename  Server will exit if this specified path and file doesn't exist" << std::endl
          << std::endl;

    if (configuration_error == "HELP") {
        std::cout << usage.str();
        return EXIT_SUCCESS;  // since they asked for help
    } else {
        std::cerr << std::endl <<  configuration_error << std::endl;
        std::cerr << usage.str();
        return EXIT_FAILURE;
    }

}



}