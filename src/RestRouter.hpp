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

#ifndef NUCLEUS_RESTROUTER_HPP
#define NUCLEUS_RESTROUTER_HPP

#include "Platform.hpp"
#include "restinio/all.hpp"
#include "json.hpp"

using namespace restinio;

namespace nucleus {


class RestServer;     // fwd declaration
class RestServerPlus; // fwd declaration, or will be empty if not linked iwth RestServerPlus from Nucleus+

class RouteOptions {

friend class RestRouter;
friend class RestRouterPlus;

public:
    spdlog::level::level_enum log_level {spdlog::level::debug};
protected:
    explicit RouteOptions() = default;
    size_t option_set_id = 0;
private:

    //RouteOptions(const RouteOptions&)               = delete; // Copy
    //RouteOptions(RouteOptions&&)                    = delete; // Move
    //RouteOptions& operator= (const RouteOptions & ) = delete; // Copy Assign
    //RouteOptions& operator= (RouteOptions && )      = delete; // Move assign

};

using req_t = restinio::request_handle_t;
using params_t = restinio::router::route_params_t;
using resp_t = nlohmann::json;
using route_options_t = RouteOptions*;
using route_callback_t = std::function<restinio::http_status_line_t(const req_t &req,
                                                                    const params_t &params,
                                                                    const session_t& session,
                                                                    resp_t &resp)>;

using route_callback_simple3_t = std::function<restinio::http_status_line_t(const req_t &req,
                                                                    const params_t &params,
                                                                    resp_t &resp)>;

using route_callback_simple2_t = std::function<restinio::http_status_line_t(const req_t &req,
                                                                                resp_t &resp)>;
/*
using route_callback_simple1_t = std::function<restinio::http_status_line_t(const req_t &req)>;
*/

using basic_auth_t = std::tuple<restinio::http_status_line_t, std::string>;



/**
 * RestRouter provides a routing service to RestServer.
 * @see Enhancements -
 */

class RestRouter {


friend class RestServer;
friend class RestServerPlus;

public:

    explicit RestRouter(Logging* logging);

    virtual ~RestRouter();
    RestRouter()                                = delete; // Default constructor
    RestRouter(const RestRouter&)               = delete; // Copy
    RestRouter(RestRouter&&)                    = delete; // Move
    RestRouter& operator= (const RestRouter & ) = delete; // Copy Assign
    RestRouter& operator= (RestRouter && )      = delete; // Move assign

    void add(restinio::http_method_id_t method, const std::string& route_path, route_options_t options, route_callback_t callback);
    // simplified versions of add -
    void add(restinio::http_method_id_t method, const std::string& route_path, route_callback_t callback);
    void add(restinio::http_method_id_t method, const std::string& route_path, route_callback_simple3_t callback);
    void add(restinio::http_method_id_t method, const std::string& route_path, route_callback_simple2_t callback);

    virtual route_options_t custom_route_options();
    virtual route_options_t default_route_options();

    void non_matched_request_handler(restinio::router::non_matched_request_handler_t handler);

    template< typename Method_Matcher >
    void add_handler_direct( Method_Matcher && method_matcher, string_view_t route_path,
                      restinio::router::express_request_handler_t handler )
    {
        log->debug("RestRouter adding direct route {}", route_path);
        router->add_handler( std::forward<Method_Matcher>(method_matcher), route_path, path2regex::options_t{},
                std::move( handler ) );
    }


protected:

    spdlog::logger* log;

    using router_t = restinio::router::express_router_t<>;
    using router_ptr_t = std::unique_ptr<router_t>;

    router_ptr_t router = std::make_unique<router_t>();

    virtual void RequestPreCallback(const restinio::request_handle_t &req,
                                    const route_options_t& options,
                                    session_t session);

    std::map<std::string, size_t> route_option_map;           // this maps routes to option object ids
    void link_route_options(const restinio::http_method_id_t& method, const std::string& route_path, route_options_t options_arg);
    virtual route_options_t make_route_options(bool custom);
    virtual route_options_t get_route_options(const req_t& req, const params_t& params);

    virtual Session* get_session(const req_t& req, const route_options_t& options);

private:

    //virtual void RegisterDefaultRoutes();

    restinio::request_handling_status_t Request(const restinio::request_handle_t &req,
                                                const restinio::router::route_params_t &params,
                                                route_callback_t callback);

    void RequestStart(const restinio::request_handle_t &req,
                      const route_options_t& options,
                      std::chrono::time_point<std::chrono::high_resolution_clock>& start);

    void RequestEnd(const restinio::request_handle_t &req,
                    const route_options_t& options,
                    const std::chrono::time_point<std::chrono::high_resolution_clock>& start,
                    session_t session,
                    const nlohmann::json& res,
                    const restinio::http_status_line_t& status);

    Session anonymous_session{};

    std::map<size_t, nucleus::RouteOptions> route_options;    // this holds route option objects

    std::tuple<std::uint16_t, std::string> http_error_parse(const std::string& message);

    restinio::router::non_matched_request_handler_t m_non_matched_request_handler;
};


class http_error : public std::exception {

public:
    http_error() : http_error(0, "RestServer::unknown http_error") {};
    http_error(std::uint16_t status_code, const std::string& msg) : message(std::to_string(status_code) + " " + msg) {};
    explicit http_error(const http_status_line_t& status) : message(std::to_string(status.status_code().raw_code()) + " " + status.reason_phrase()) {};
    explicit http_error(const http_status_line_t& status, const std::string& msg) :
            message(std::to_string(status.status_code().raw_code()) + " " + msg) {};

    [[nodiscard]] const char *what() const noexcept override {
        return message.c_str();
    };

private:
    std::string message;

};


}
#endif //NUCLEUS_RESTROUTER_HPP


