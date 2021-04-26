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
#include <sstream>
#include "spdlog/spdlog.h"

namespace nucleus {
/**
 * Manages the configuration context.
 * @see Enhancements - https://github.com/axomem/nucleus/issues/55
 */

class Config {


public:

    Config();
    explicit Config(const std::string& app_name);
    explicit Config(int argc, char *argv[]);
    virtual ~Config() = default;

    void config_parse_args(int argc, char *argv[]);
    static int print_help(const std::exception& configuration_error);
    static int print_help(const std::string& configuration_error);
    // These are the config values to use.
    // Also add to Config.cpp and nucleus.conf.template

    std::string app_name;

    // For logging
    spdlog::level::level_enum log_level = spdlog::level::info;
    std::string log_file;

    // For PoolManager - main pool file
    std::string pool_main_file;
    size_t pool_main_size = (size_t) 1024*1024*1024*1; // 1GB

    // For ReSTServer
    bool rest_disable = false;
    unsigned short rest_port = 8080;
    std::string rest_address = "127.0.0.1";
    size_t rest_threads = 4;

    // For Testing or without ReST Server
    std::string condition_path;

    static unsigned short parse_ip_port(const std::string& port);
    static bool parse_bool(const std::string& bool_arg);
    static std::string to_string(bool bool_arg);

private:

    /// Set defaults based on name
    void set_name(const std::string& name);

    /// Convert a set of args from command line into a string stream with EOL chars
    static std::stringstream args_to_stringstream(int argc, char *argv[]);

    /// Prototype of the callback for the config line processor
    typedef std::function<void(std::string section, std::string name, std::string value)> handler_t;

    /// Parse a stringstream of config items separated by EOL
    void ini_parse_stringstream(std::stringstream& stream, const handler_t& callback);

    /// Parse the given conf file
    void ini_parse_file(const std::string& filename, const handler_t& callback);

protected:

    /**
     * Process a given line from a config file and set to correct vars
     * @param section - the conf section like [section]. Not currently used but may be expanded later.
     * @param name Name of the config item
     * @param value Value of the config item
     */

    virtual void handler(const std::string& section, const std::string& name, const std::string& value);

    /// Trim all space chars on the left
    ///@copyright trim function based on http://stackoverflow.com/a/217605
    template <class CharT>
    inline void ltrim(std::basic_string<CharT> & s) {
        s.erase(s.begin(),
                std::find_if(s.begin(), s.end(),
                             [](int ch) { return !std::isspace(ch); }));
    }

    /// Trim all space chars on the right
    ///@copyright trim function based on http://stackoverflow.com/a/217605
    template <class CharT>
    inline void rtrim(std::basic_string<CharT> & s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                             [](int ch) { return !std::isspace(ch); }).base(),
                s.end());
    }

};


}

#endif //NUCLEUS_CONFIG_HPP
