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

#define WIN32_LEAN_AND_MEAN
#define ASIO_HEADER_ONLY 1
#define ASIO_STANDALONE 1

#include "Platform.hpp"
#include "restinio/all.hpp"
#include "json.hpp"
#include "RestRouter.hpp"

using namespace restinio;

namespace nucleus {


using session_t = Session*;

using basic_auth_t = std::tuple<restinio::http_status_line_t, std::string>;


/**
 * RestServer provides a ReST-ful interface to persistent applications.
 * @see Enhancements - https://github.com/axomem/nucleus/issues/56
 */

class RestServer {

public:

    RestServer(Config* config, Logging* logging);

    RestServer(Logging* logging, std::string address, unsigned short port = 80, size_t threads = 4);
    virtual ~RestServer();
    RestServer()                                = delete; // Default constructor
    RestServer(const RestServer&)               = delete; // Copy
    RestServer(RestServer&&)                    = delete; // Move
    RestServer& operator= (const RestServer & ) = delete; // Copy Assign
    RestServer& operator= (RestServer && )      = delete; // Move assign

    void Start();
    void Stop();

    [[nodiscard]] std::string last_error() const;

    template <typename ...Args>
    [[deprecated("Use route->add")]]
    void RegisterRoute(Args && ...args)
    {
        rest_router->add(std::forward<Args>(args)...);
    }

    virtual RestRouter* routes();

protected:

    spdlog::logger* log;

    using router_t = restinio::router::express_router_t<>;
    using router_ptr_t = std::unique_ptr<router_t>;

    virtual request_handling_status_t route_handler( request_handle_t req);
    virtual void RegisterDefaultRoutes();

private:

    std::string address;
    unsigned short port;
    size_t threads;

    using my_traits_t = restinio::traits_t<
            restinio::asio_timer_manager_t,
            restinio::null_logger_t,
            restinio::default_request_handler_t>;

    using server_t = restinio::http_server_t<my_traits_t>;
    using server_ptr_t = std::shared_ptr<server_t>;
    server_ptr_t server;

    using runner_t = restinio::on_pool_runner_t<restinio::http_server_t<my_traits_t>>;
    using runner_ptr_t = std::unique_ptr<runner_t>;
    runner_ptr_t runner;

    std::string m_last_error;

    std::unique_ptr<RestRouter> rest_router;

    Session anonymous_session{};

};


}
#endif //NUCLEUS_REST_HPP


