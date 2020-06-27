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
#include "json.hpp"

using namespace restinio;

namespace nucleus {

    using req_t = restinio::request_handle_t;
    using params_t = restinio::router::route_params_t;
    using resp_t = nlohmann::json;
    using route_callback_t = std::function<restinio::http_status_line_t(const req_t &req,
                                                                        const params_t &params,
                                                                        resp_t &resp)>;

    using basic_auth_t = std::tuple<restinio::http_status_line_t, std::string>;

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
 * @see Enhancements - https://github.com/axomem/nucleus/issues/56
 */

/**
 * RestServer provides a ReST-ful interface to persistent applications.
 * @see Enhancements - https://github.com/axomem/nucleus/issues/56
 */

class RestServer {

public:
    explicit RestServer( const CTX& ctx_arg, const std::string& address = "localhost", unsigned short port = 80,
                         size_t threads = 4);
    ~RestServer();
    RestServer(const RestServer&)                = delete; // Copy
    RestServer(RestServer&&)                     = delete; // Move
    RestServer& operator= (const RestServer & ) = delete; // Copy Assign
    RestServer& operator= (RestServer && )      = delete; // Move assign

    void Start();
    void Stop();

    std::string last_error() const;

    enum http_verb {GET, HEAD, POST, PUT, DELETE, CONNECT, OPTIONS, TRACE, PATCH, MAX_HTTP_VERB};

    struct RouteOptions {
        spdlog::level::level_enum log_level {spdlog::level::info};
    };

    void RegisterRoute(restinio::http_method_id_t method, const std::string& route_path, route_callback_t callback);
    void RegisterRoute(restinio::http_method_id_t method, const std::string& route_path, RouteOptions options, route_callback_t callback);

    /**
     * Wrapper for a standard ReST API call
    **/

private:
    CTX ctx;

    std::string address;
    unsigned short port;
    size_t threads;

    using router_t = restinio::router::express_router_t<>;
    using router_ptr_t = std::unique_ptr<router_t>;

    router_ptr_t router = std::make_unique<router_t>();

    using my_traits_t = restinio::traits_t<
            restinio::asio_timer_manager_t,
            restinio::null_logger_t,
            restinio::router::express_router_t<>>;

    using server_t = restinio::http_server_t<my_traits_t>;
    using server_ptr_t = std::shared_ptr<server_t>;
    server_ptr_t server;

    using runner_t = restinio::on_pool_runner_t<restinio::http_server_t<my_traits_t>>;
    using runner_ptr_t = std::unique_ptr<runner_t>;
    runner_ptr_t runner;

    std::string m_last_error;

    void RegisterDefaultRoutes();

    void RequestStart(const restinio::request_handle_t &req,
                      const RouteOptions& options,
                      std::chrono::time_point<std::chrono::high_resolution_clock>& start);

    restinio::request_handling_status_t Request(const restinio::request_handle_t &req,
                                                const restinio::router::route_params_t &params,
                                                route_callback_t callback,
                                                const RouteOptions& options);

    void RequestEnd(const restinio::request_handle_t &req,
                    const RouteOptions& options,
                    const std::chrono::time_point<std::chrono::high_resolution_clock>& start,
                    const nlohmann::json& res,
                    const restinio::http_status_line_t& status);


};


}
#endif //NUCLEUS_REST_HPP


