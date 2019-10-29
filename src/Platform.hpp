// AxoMem Nucleus - for developing Persistent Applications
// Copyright (C) 2018-2019 AxoMem Pte Ltd.

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
#include <asio.hpp>

// Issue with FMT - spdlog and restinio both use?
#define FMT_HEADER_ONLY

// PMDK common includes
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/pool.hpp>
#include <libpmemobj++/transaction.hpp>
#include <libpmemobj++/p.hpp>
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/make_persistent_array.hpp>
#include <libpmemobj++/container/vector.hpp>
#include <libpmemobj++/container/concurrent_hash_map.hpp>
#include <libpmemobj++/container/string.hpp>
#include <libpmemobj++/utils.hpp>

// Nucleus common includes
#include "Logging.hpp"
#include "Config.hpp"
#include "Utilities.hpp"

#endif //NUCLEUS_PLATFORM_HPP
