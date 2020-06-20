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


RestServerRouter::RestServerRouter() {

    Logging::log()->trace("ReST Server configuring default routes");

    router->non_matched_request_handler(
            [](auto req){
                return req->create_response(status_not_found()).connection_close().done();
            });
    Logging::log()->trace("ReST Server default routes done");
}

std::unique_ptr<router::express_router_t<>>
RestServerRouter::getRouter(){
    Logging::log()->trace("RestServer GetRouter being called");
    assert(router != nullptr && "Unable to get Router since its already out for route population or server has started");

    return std::move(router);
}

void
RestServerRouter::setRouter(std::unique_ptr<router::express_router_t<>> router_arg) {
    Logging::log()->trace("RestServer SetRouter being called");
    assert(router == nullptr && "Unable to set Router since its already active. Check logic.");

    router = std::move(router_arg);
}

RestServerRouter &
RestServerRouter::getRestServerRouter() {
    static RestServerRouter singleton_instance;
    return singleton_instance;
}

// --------------------
// TODO - also need to catch 'std::system_error' bind: Address already in use here. ideally abort Nucleus?

RestServer::RestServer(std::unique_ptr<router::express_router_t<>> router,
                       const std::string& address_arg, unsigned short port_arg,
                       size_t threads_arg) :

    // This creates the server object but does not start it
    my_server { restinio::own_io_context(),
                restinio::server_settings_t< my_traits_t >{}
                .port( port_arg )
                .address( address_arg )
                .separate_accept_and_create_connect(true)
                .request_handler( std::move(router) )
    }

{
    Logging::log()->info("ReST Server starting at http://{}:{} with {} threads across {} CPUs",
                         config::rest_address, config::rest_port, config::rest_threads,
                         std::thread::hardware_concurrency());

    restinio_control_thread = std::thread { [this, threads_arg] {
            restinio::run( restinio::on_thread_pool(threads_arg, restinio::skip_break_signal_handling(),
                                                           my_server));
        }
    };

}

RestServer::~RestServer() {

    Logging::log()->debug("RestServer is being shut down");

    restinio::initiate_shutdown(my_server);
    restinio_control_thread.join();
    Logging::log()->info("RestServer has shut down");
}

