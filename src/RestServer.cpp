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

#include "RestServer.hpp"
#include <regex>
#include <typeinfo>  // still needed for typeid

using nlohmann::json;
using namespace restinio;


namespace nucleus {

RestServer::RestServer(const CTX& ctx_arg, const std::string& address_arg, unsigned short port_arg, size_t threads_arg) :
        ctx(ctx_arg), address(address_arg), port(port_arg), threads(threads_arg)
{

    ctx->log->trace("Rest Server is constructing");

    RegisterDefaultRoutes();

}

void
RestServer::Start() {

    server = std::make_unique<server_t> ( restinio::own_io_context(),
                restinio::server_settings_t< my_traits_t >{}
                .port( port )
                .address( address )
                .separate_accept_and_create_connect(true)
                .request_handler( std::move(router)));

    runner = std::make_unique<runner_t>(threads, *server);

    // future for ReST server start exception
    std::promise<void> run_promise;
    auto run_future = run_promise.get_future();

    // Start the server
    runner->start(
        [&run_promise]() noexcept {
            run_promise.set_value();
        },
        [&run_promise](std::exception_ptr ex) noexcept {
            run_promise.set_exception(std::move(ex));
        }
    );

    try {
        run_future.get();
        ctx->log->debug("ReST Server started with {} threads across {} CPUs",
                       threads,std::thread::hardware_concurrency());
        ctx->log->info("*** Ping ReST URL is http://{}:{}/api/v1/ping", address, port);

    } catch (const std::system_error& e) {
        m_last_error = e.what();
        ctx->log->critical("ReSTServer() starting error: {}", e.what());
    }


}

void
RestServer::Stop() {

    ctx->log->info("RestServer is being shut down");

    runner->stop();
    runner->wait();

    ctx->log->debug("RestServer has shut down");
}

RestServer::~RestServer() {
    ctx->log->debug("RestServer is closing");
}

std::string RestServer::last_error() const {
    return m_last_error;
}

void RestServer::RegisterDefaultRoutes() {

    ctx->log->trace("ReST Server configuring default routes");

    router->non_matched_request_handler([](auto req){
        // Retuns a 404 if route not found
        return req->create_response(status_not_found()).connection_close().done();

    });

    ctx->log->trace("ReST Server default routes done");

}

void
RestServer::RegisterRoute(restinio::http_method_id_t method,
                          const std::string& route_path,
                          route_callback_t callback) {
    RouteOptions options;
    RegisterRoute(method, route_path, options, callback);
}

void
RestServer::RegisterRoute(restinio::http_method_id_t method,
                          const std::string& route_path,
                          RouteOptions options,
                          route_callback_t callback) {

    ctx->log->debug("RestServer registering route {} {}", method.c_str(),  route_path);

    router->add_handler(method, route_path,
                        [this, callback, options](const restinio::request_handle_t &req,
                                                const restinio::router::route_params_t &params) {

        // Note - we are taking a deliberate copy of callback & options here, otherwise we lose the ref
        // when the calling function exits. Could use std::copy?

        return Request(req, params, callback, options);

    });

}

restinio::request_handling_status_t
RestServer::Request(const restinio::request_handle_t &req,
                    const restinio::router::route_params_t &params,
                    route_callback_t callback,
                    const RouteOptions& options) {

    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    RequestStart(req, options, start);

    // Json response object
    nlohmann::json res = "{}"_json;
    res["response"] = "{}"_json;

    restinio::http_status_line_t status;

    try {

        ctx->log->trace("Calling callback");
        status = callback(req, params, res);
        ctx->log->trace("Back from callback. Status is {}", status.status_code().raw_code());

    } catch ( const std::exception &exc) {

        status = restinio::status_internal_server_error();
        res["response"]["reason"] = fmt::format("Server error: {} ({})", exc.what(), typeid(exc).name());

    }

    RequestEnd(req, options, start, res, status);

    // return the response;
    return req->create_response(status)
            .append_header( restinio::http_field_t::access_control_allow_origin, "*" )
            .set_body( res.dump())
            .done();

}

void RestServer::RequestStart(const restinio::request_handle_t &req,
                              const RouteOptions& options,
                              std::chrono::time_point<std::chrono::high_resolution_clock>& start) {

        start = std::chrono::high_resolution_clock::now();

        if (ctx->config->log_level == spdlog::level::trace) {

            // Log headers for debugging
            std::stringstream headers;
            for(const auto & it : req->header()) {headers << it.name() << ":" << it.value() << "|";}

            ctx->log->trace("RESTAPI START: {} || {} || {}", req->remote_endpoint(), headers.str(), req->body());
        }
}

void
RestServer::RequestEnd(const restinio::request_handle_t &req,
                       const RouteOptions& options,
                       const std::chrono::time_point<std::chrono::high_resolution_clock>& start,
                       const json& res,
                       const restinio::http_status_line_t& status) {

    auto path = (std::string) req->header().path();

    auto log_level = status.status_code() == restinio::status_internal_server_error().status_code() ? spdlog::level::err : options.log_level;

    // Calculate response time
    std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
    auto elapsed_ms = (long) std::round(elapsed.count() * 1000);

    // Log request
    std::string log_msg = res["response"].contains("reason") ? res["response"]["reason"] : "OK";

    // Add separate logger - https://github.com/axomem/nucleus/issues/62
    ctx->log->log(log_level, "{} {}__{}  {} {}?{} {} {} {}", req->remote_endpoint(),
                  req->header().method(), path, req->header().query(),
                  status.status_code().raw_code(), elapsed_ms, log_msg);

}

}