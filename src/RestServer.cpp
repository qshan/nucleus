// AxoMem Nucleus - for developing Persistent Applications
// Copyright (C) 2018-2019 AxoMem Pte Ltd.

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License Version 2 (only)
// as published by the Free Software Foundation.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License Version 2 for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, see http://www.gnu.org/licenses/

// TODO - how to add .append_header( restinio::http_field_t::access_control_allow_origin, "*" ) across all
// TODO - how to write custom logger that logs info/warning/errors to our logger.

#include "RestServer.hpp"
#include "restinio/all.hpp"
#include "json.hpp"

using nlohmann::json;
using namespace restinio;
using namespace nucleus;

template<typename T>
std::ostream & operator<<(std::ostream & to, const optional_t<T> & v) {
// TODO - check if this is needed from restinio after finishing routes
    if(v) to << *v;
    return to;
}

RestServerRouter::RestServerRouter()
        : router{std::make_unique<router::express_router_t<>>()}
{

    Logging::log()->trace("ReST Server configuring default routes");

    router->http_get(
            R"(/api/v1/ready)",
            [](auto req, auto params) {
                json j = "{}"_json;
                j["data"] = "{}"_json;
                j["response"]["message"] = "Ready command received successfully";
                return req->create_response()
                        .append_header( restinio::http_field_t::access_control_allow_origin, "*" )
                        .set_body( j.dump())
                        .done();
            });

    router->non_matched_request_handler(
            [](auto req){
                return req->create_response(status_not_found()).connection_close().done();
            });

}

std::unique_ptr<router::express_router_t<>>
RestServerRouter::getRouter(){
    Logging::log()->trace("RestServer GetRouter being called");
    if (router == nullptr) {
        throw std::logic_error("Unable to get Router since its already out for route population or server has started");
    }
    return std::move(router);
}

void
RestServerRouter::setRouter(std::unique_ptr<router::express_router_t<>> router_arg) {
    Logging::log()->trace("RestServer SetRouter being called");
    if (router != nullptr) {
        throw std::logic_error("Unable to set Router since its already active. Check logic.");
    }
    router = std::move(router_arg);
}

RestServerRouter &
RestServerRouter::getRestServerRouter() {
    static RestServerRouter singleton_instance;
    return singleton_instance;
}

// --------------------

RestServer::RestServer(std::unique_ptr<router::express_router_t<>> router)
    : my_server {
            restinio::own_io_context(),
            restinio::server_settings_t< my_traits_t >{}
                .port( config::rest_port )
                .address( config::rest_address )
                .separate_accept_and_create_connect(true)
                .request_handler( std::move(router) )

            }

{
    Logging::log()->info("ReST Server configured at http://{}:{} with {} threads across {} CPUs",
                         config::rest_address, config::rest_port, config::rest_threads, std::thread::hardware_concurrency());

    restinio_control_thread = std::thread { [&] {
        restinio::run( restinio::on_thread_pool( config::rest_threads, restinio::skip_break_signal_handling(), my_server));
    }
    };

}

RestServer::~RestServer() {

    Logging::log()->debug("RestServer is being shut down");

    restinio::initiate_shutdown(my_server);
    restinio_control_thread.join();
    Logging::log()->info("RestServer has shut down");
}

