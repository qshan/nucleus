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
#include "Logging.hpp"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/async.h"

using namespace nucleus;

Logging::Logging(const std::string& name, const std::string& log_file, const spdlog::level::level_enum log_level)
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::trace);

    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_file, 1024*1024 * 20, 10);
    file_sink->set_level(spdlog::level::trace);

    std::array<spdlog::sink_ptr, 2> sinks {console_sink, file_sink};

    // Note - if missing log entries, need to increase these numbers
    spdlog::init_thread_pool(LOG_SLOTS_K, LOG_THREADS);
    mylog = std::make_shared<spdlog::async_logger>(name, sinks.begin(), sinks.end(),
            spdlog::thread_pool(), spdlog::async_overflow_policy::overrun_oldest);

    spdlog::register_logger(mylog);

    mylog->set_level(log_level);

    mylog->flush_on(spdlog::level::debug);
    spdlog::flush_every(std::chrono::seconds(5));

    mylog->info("Logging has been initialised with name {} and loglevel {} and saved to {}",
                name, spdlog::level::to_string_view(log_level), log_file);
    mylog->debug("Logging is async using {} threads with {} slots and a DROP policy if out of slots. "
                 "If missing log entries, adjust these defines (recompile needed)", LOG_THREADS, LOG_SLOTS_K);

    // This line must be retained if using the Nucleus Open Source license.
    mylog->info("Built with Nucleus. See https://github.com/axomem/nucleus for more info, "
                "and follow us on Twitter @axomemio for updates");
    // The line above must be retained if using the Nucleus Open Source license.
}

const std::shared_ptr<spdlog::logger> &
Logging::get_logger() const {
    return mylog;
}

