//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//

#ifndef NUCLEUS_PLATFORM_HPP
#define NUCLEUS_PLATFORM_HPP

// PMDK #WIN64 issue - check on linux?
#define PMEMOBJ_OFFSETOF_WA

// Restinio & ASIO.. This is to avoid ASIO Winsock already included issue
#define WIN32_LEAN_AND_MEAN

// Issue with FMT - spdlog and restinio both use?
#define FMT_HEADER_ONLY

// PMDK common includes
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/pool.hpp>
#include <libpmemobj++/transaction.hpp>
#include <libpmemobj++/p.hpp>
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/make_persistent_array.hpp>
#include <libpmemobj++/transaction.hpp>
#include <libpmemobj++/experimental/vector.hpp>
#include <libpmemobj++/experimental/concurrent_hash_map.hpp>
#include <libpmemobj++/experimental/string.hpp>

// Nucleus common includes
#include "Logging.hpp"
#include "Config.hpp"
#include "Utilities.hpp"

// App include
#include "MyApp.hpp"

namespace nucleus{

    int NucleusRun(int argc, char *argv[]);

};

#endif //NUCLEUS_PLATFORM_HPP
