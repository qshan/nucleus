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

// *****************************************************************
// For help with Restinio, see https://stiffstream.com/en/docs.html
// *****************************************************************

#ifndef NUCLEUS_REST_HPP
#define NUCLEUS_REST_HPP

#define ASIO_HEADER_ONLY 1
#define ASIO_STANDALONE 1

#include "Platform.hpp"
#include "restinio/all.hpp"

namespace nucleus {

/**
 * RestServerRouter manages the Router for URLs for RestServer.
 * The router controls which functions to call for various urls like `/api/v1/ready`.
 *
 * Routes can only be set during application startup, typically as part of a `Start()` function.
 * 1. Use `auto router = getRestServerRouter().getRouter()` to get the router.
 * 2. Set routes using router->http_get(...) (see HelloWorld for basic examples of get and put verbs)
 * 3. **important** Return the router via `getRestServerRouter().setRouter(std::move(router));`
 *
 * @note If the router is not returned, the RestServer will not start and will fail with exception
 *
 */
class RestServerRouter {

public:
    explicit RestServerRouter();
    std::unique_ptr<restinio::router::express_router_t<>> getRouter();
    void setRouter(std::unique_ptr<restinio::router::express_router_t<>>);
    static RestServerRouter & getRestServerRouter();

private:
    using router_t = restinio::router::express_router_t<>;
    std::unique_ptr<router_t> router = std::make_unique<router_t>();
};
/**
 * RestServer provides a ReST-ful interface to persistent applications.
 */
class RestServer {

public:
    explicit RestServer( std::unique_ptr<restinio::router::express_router_t<>> router,
                         const std::string& address = "localhost", unsigned short port = 80,
                         size_t threads = 4);
    ~RestServer();
    RestServer(const RestServer&) = delete;
    RestServer &operator=(const RestServer &) = delete;

private:

    using my_traits_t = restinio::traits_t<
            restinio::asio_timer_manager_t,
            restinio::null_logger_t,
            restinio::router::express_router_t<>>;
    using my_server_t = restinio::http_server_t<my_traits_t>;
    my_server_t my_server;
    std::thread restinio_control_thread;
};


}
#endif //NUCLEUS_REST_HPP


