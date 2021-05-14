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

#ifndef NUCLEUS_PLATFORM_HPP
#define NUCLEUS_PLATFORM_HPP

// PMDK #WIN64 issue - check on linux?
#define PMEMOBJ_OFFSETOF_WA

// Restinio & ASIO.. This is to avoid ASIO Winsock already included issue
#define WIN32_LEAN_AND_MEAN

#define ASIO_HEADER_ONLY 1
#define ASIO_STANDALONE 1
#define ASIO_DISABLE_NOEXCEPT 1

#include <asio.hpp>

// Issue with FMT - spdlog and restinio both use?
#define FMT_HEADER_ONLY

#include "fmt/format.h"

#include "json.hpp"

// PMDK common includes
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/pool.hpp>
#include <libpmemobj++/transaction.hpp>
#include <libpmemobj++/p.hpp>
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/make_persistent_array.hpp>
#include <utility>
#include <libpmemobj++/container/vector.hpp>
#include <libpmemobj++/container/concurrent_hash_map.hpp>
#include <libpmemobj++/container/string.hpp>
#include <libpmemobj++/utils.hpp>
#include <libpmemobj++/experimental/v.hpp>
#include <libpmemobj++/experimental/inline_string.hpp>

// Nucleus common includes
#include "Logging.hpp"
#include "Config.hpp"
#include "Utilities.hpp"

namespace nucleus {

using namespace pmem::obj;
using nlohmann::json;

class RestServer; // fwd declaration

/// @brief Provides access to common execution components
class Context {

public:
    Config* config;
    Logging* logging;
    spdlog::logger* log;  /// @brief direct access to default logger for ease of use
    RestServer* rest_server;
};

using CTX = Context*;

}

struct Session {
    std::string username = {"anonymous"};
};
using session_t = Session*;

namespace nucleus::exceptions {

/**
 * Exception for an error in the Nucleus application manager.
 * This may be due to incorrect or failed initialisation of the AppManager instance
 */
class app_error : public std::exception {

public:
    app_error() : app_error("nucleus::app_error") {};
    explicit app_error(std::string msg) : message(std::move(msg)) {};
    const char *what() const noexcept override {
        return message.c_str();
    };

private:

    std::string message;

};

}

#endif //NUCLEUS_PLATFORM_HPP
