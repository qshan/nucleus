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


RestServer::RestServer(Config* config, Logging* logging) :
        RestServer(logging, config->rest_address, config->rest_port, config->rest_threads)
{ }

RestServer::RestServer(Logging* logging, std::string address_arg, unsigned short port_arg, size_t threads_arg) :
        log(logging->get_logger()), address(std::move(address_arg)), port(port_arg), threads(threads_arg),
        rest_router(std::make_unique<RestRouter>(logging))
{
    log->trace("RestServer::() called as {}:{} with {} threads", address, port, threads);
}

RestServer::~RestServer() {
    log->trace("RestServer::~() called");
}

void
RestServer::Start() {

    RegisterDefaultRoutes();

    server = std::make_unique<server_t> ( restinio::own_io_context(),
                restinio::server_settings_t< my_traits_t >{}
                .port( port )
                .address( address )
                .separate_accept_and_create_connect(true)
                .request_handler([this](auto req) {
                    return route_handler(std::move(req));
                }));

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
        log->debug("ReST Server started with {} threads across {} CPUs",
                       threads,std::thread::hardware_concurrency());
        log->info("*** Ping ReST URL is http://{}:{}/api/v1/ping", address, port);

    } catch (const std::system_error& e) {
        m_last_error = e.what();
        log->critical("ReSTServer() starting error: {}", e.what());
    }

}

void
RestServer::Stop() {

    log->info("RestServer is being shut down");

    runner->stop();
    runner->wait();

    log->debug("RestServer has shut down");
}

std::string
RestServer::last_error() const {
    return m_last_error;
}

void
RestServer::RegisterDefaultRoutes()
{
    log->trace("ReST Server configuring default routes");

    auto options = rest_router->custom_route_options();
    options->log_level = spdlog::level::info;

    rest_router->add(restinio::http_method_get(), "/api/v1/ping", options,
             [this](const req_t &req, const params_t &params, const session_t &session, resp_t &resp) {
        log->trace("RestServer returning ping");
        return restinio::status_ok();
    });

    rest_router->non_matched_request_handler([this](auto req){
        // Returns a 404 if route not found
        log->trace("Using RestServer nmrh");
        return req->create_response(status_not_found()).connection_close().done();
    });

    log->trace("ReST Server default routes done");
}

request_handling_status_t
RestServer::route_handler(request_handle_t req)
{
    log->trace("Using base route handler");
    return (*rest_router->router)(std::move(req));
}

RestRouter*
RestServer::routes()
{
    return rest_router.get();
}


}