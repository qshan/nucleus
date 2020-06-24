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
#include "restinio/all.hpp"
#include "json.hpp"

using nlohmann::json;
using namespace restinio;
using namespace nucleus;


RestServerRouter::RestServerRouter(const CTX& ctx_arg) : ctx(ctx_arg) {

    ctx->log->trace("ReST Server configuring default routes");

    router->non_matched_request_handler(
            [](auto req){
                return req->create_response(status_not_found()).connection_close().done();
            });
    ctx->log->trace("ReST Server default routes done");
}

std::unique_ptr<router::express_router_t<>>
RestServerRouter::getRouter(){
    ctx->log->trace("RestServer GetRouter being called");
    assert(router != nullptr && "Unable to get Router since its already out for route population or server has started");

    return std::move(router);
}

void
RestServerRouter::setRouter(std::unique_ptr<router::express_router_t<>> router_arg) {
    ctx->log->trace("RestServer SetRouter being called");
    assert(router == nullptr && "Unable to set Router since its already active. Check logic.");

    router = std::move(router_arg);
}


// --------------------
// TODO - also need to catch 'std::system_error' bind: Address already in use here. ideally abort Nucleus?

RestServer::RestServer(const CTX& ctx_arg, std::unique_ptr<restinio::router::express_router_t<>> router,
                       const std::string& address_arg, unsigned short port_arg,
                       size_t threads_arg) :
    ctx(ctx_arg),
    // This creates the server object but does not start it
    my_server { restinio::own_io_context(),
                restinio::server_settings_t< my_traits_t >{}
                .port( port_arg )
                .address( address_arg )
                .separate_accept_and_create_connect(true)
                .request_handler( std::move(router) )
    },
    runner{threads_arg, my_server}

{
    // future for ReST server exception
    std::promise<void> run_promise;
    auto run_future = run_promise.get_future();

    // Start the server
    runner.start(
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
                       threads_arg,std::thread::hardware_concurrency());
        ctx->log->info("*** Ping ReST URL is http://{}:{}/api/v1/ping", address_arg, port_arg);

    } catch (const std::system_error& e) {
        last_error_msg = e.what();
        ctx_arg->log->critical("ReSTServer() starting error: {}", e.what());
    }

}

RestServer::~RestServer() {

    ctx->log->info("RestServer is being shut down");

    runner.stop();
    runner.wait();

    ctx->log->debug("RestServer has shut down");
}

std::string RestServer::last_error() const {
    return last_error_msg;
};
