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

#ifndef NUCLEUS_LOGGING_H
#define NUCLEUS_LOGGING_H

// #define SPDLOG_FMT_EXTERNAL
#include "spdlog/spdlog.h"

namespace nucleus {


constexpr size_t LOG_SLOTS_K = 16 * 1024;
constexpr size_t LOG_THREADS = 2;

// forward declaration
template<class T>
class Nucleus;

/**
 * Logging controls Nucleus logging to console and log files
 * This is a singleton class that can be accessed via `Logging::log()`
 *
 * Logging uses the [spdlog library](https://github.com/gabime/spdlog/wiki/1.-QuickStart) for logging.
 *
 * ### Examples
 * `Logging::log()->debug("This is a debug message")` \n
 * `Logging::log()->trace("This is a formatted message with {} and {}", string_var, int_var)`
 *
 * Logging uses [fmt::format](https://fmt.dev/latest/index.html) for string formatting.
 *
 * ### Configuration
 * Configuration is managed by two Config items:
 * * **Config::log_file:** The path to the log file
 * * **Config::log_level:** The log level to use. Levels are `critical`, `error`, `warn`, `info`, `debug`, `trace`
 *
 */

class Logging {

template<class T>
friend class Nucleus;

public:
    static std::shared_ptr<spdlog::logger> &log();

private:
    explicit Logging(const std::string &name); // hide constructor - use init for singleton
    static Logging *init();

    static Logging *logger;
    static std::shared_ptr<spdlog::logger> mylog;  // this is the

    Logging(const Logging &) = delete;  // hide copy constructor
    Logging &operator=(const Logging &) = delete; // hide assignment operator

};


}
#endif //NUCLEUS_LOGGING_H
