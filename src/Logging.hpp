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

constexpr size_t LOG_SLOTS_K = 16 * 1024;
constexpr size_t LOG_THREADS = 2;

class Logging {

public:
    static Logging *init();
    static std::shared_ptr<spdlog::logger> &log();

private:
    explicit Logging(const std::string &name); // hide constructor - use init for singleton
    static Logging *logger;

    static std::shared_ptr<spdlog::logger> mylog;  // this is the

    Logging(const Logging &);  // hide copy constructor
    Logging &operator=(const Logging &); // hide assignment operator

};

#endif //NUCLEUS_LOGGING_H
