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

#include "Nucleus.hpp"
#include "Subclass.hpp"

using namespace nucleus;
using namespace nucleus::examples::subclass;

int
main(int argc, char *argv[]) {

    // Demonstrating manual context building

    // Load configuration manager first. Needed for log settings
    auto config = Config();
    try {
        config.config_parse_args(argc, argv);
    } catch (const std::invalid_argument &exc) {
        return Config::print_help(exc);
    }

    auto logger = Logging(&config);
    auto rest_server = RestServer(&config, &logger);

    Context ctx {&config, &logger, logger.get_logger(), &rest_server};

    return nucleus::Nucleus<SubClass>(&ctx).Run();

}

