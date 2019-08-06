//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//

// TODO - implement better tracing eg via __func__, https://github.com/gabime/spdlog/issues/235

#include <iostream>
#include "Logging.hpp"
#include "Config.hpp"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

Logging *Logging::logger = nullptr; // Singleton model - sets the internal private member
std::shared_ptr<spdlog::logger> Logging::mylog = nullptr;

Logging::Logging(const std::string &name)
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::trace);

    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(config::log_file, 1024*1024 * 20, 10);
    file_sink->set_level(spdlog::level::trace);
    std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};

    mylog = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
    mylog->set_level(config::log_level);

    mylog->info("Logging has been initialised with name {} and loglevel {} and saved to {}",
                name, spdlog::level::to_string_view(config::log_level), config::log_file);

    // This line must be retained if using the Nucleus MIT Open Source license.
    mylog->info("Built with Nucleus. See https://axomem.io for more info, and follow us on Twitter @axomemio for updates");
    // The line above must be retained if using the Nucleus MIT Open Source license.
}

Logging *
Logging::init() {
    if (logger == nullptr) {
        logger = new Logging("main");
    }
    return logger;
};

std::shared_ptr<spdlog::logger> &
Logging::log() {
    return mylog;
};

Logging::~Logging() {
    std::clog << "Closing logging " << std::endl;
    //mylog.close?
}

