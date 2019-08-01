//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//
// TODO - write custom logger that logs info/warning/errors to our logger.

#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <chrono>
#include "RestServer.hpp"
#include "PoolManager.hpp"
#include "restinio/all.hpp"
#include "json.hpp"

using nlohmann::json;
using namespace restinio;

RestServer *RestServer::restEndpoint = nullptr; // Pointer to class instance (singleton)

// TODO - check if this is needed from restinio after finishing routes
template<typename T>
std::ostream & operator<<(std::ostream & to, const optional_t<T> & v) {
    if(v) to << *v;
    return to;
}

RestServer::RestServer() {

    trigger_shutdown = false;
    using router_t = router::express_router_t<>;
    router = std::make_unique<router_t>();
    configureRoutes();

}

void RestServer::run() {
    Logging::log()->trace("rest_run is starting ReST Server");

    if (router == nullptr) {
        throw std::logic_error("Unable to start ReST Server - Router is missing. Check it has been returned to Server");
    }

    auto duration = std::chrono::duration<float>(0.5); //0.5 seconds loop duration
    try {

        // Define ReST server traits
        // For debug logging, change null_logger_t to single_threaded_ostream_logger_t
        using my_traits_t = restinio::traits_t<
                restinio::asio_timer_manager_t,
                restinio::null_logger_t,
                router::express_router_t<>>;

        // Create server instance - this does not run it yet
        using my_server_t = restinio::http_server_t< my_traits_t >;
        my_server_t server {
                restinio::own_io_context(),
                restinio::server_settings_t< my_traits_t >{}
                        .port( config::rest_port )
                        .address( "localhost" )
                        .request_handler( std::move(router) )
        };

        Logging::log()->info("ReST Server configured on port {} with {} threads across {} CPUs",
                             config::rest_port, config::rest_threads, std::thread::hardware_concurrency());

        // Run rest server on separate control thread
        std::thread restinio_control_thread{ [&server] {
            restinio::run( restinio::on_thread_pool( config::rest_threads, restinio::skip_break_signal_handling(), server));
        }
        };

        // hold the thread here until trigger_shutdown received from external source
        while (!trigger_shutdown) {
            std::this_thread::sleep_for(duration);
            std::this_thread::yield();
        }

        Logging::log()->info("rest_run is stopping ReST Server");
        restinio::initiate_shutdown(server);
        restinio_control_thread.join();
    }
    catch (std::exception& ex) {
        Logging::log()->error("Exception type {} in ReST server: {}", typeid(ex).name(), ex.what());
    }
    catch (...) {
        Logging::log()->error("ERROR in ReST server - Unknown exception.");
    }
    Logging::log()->info("ReST Server is shutting down");

}


RestServer *
RestServer::getRestServer() {
    if (restEndpoint == nullptr) {
        Logging::log()->debug("ReST Server pointer not yet initialised - creating Rest Server object");
        restEndpoint = new RestServer();
        Logging::log()->trace("ReST Server Initialised");
    }
    return restEndpoint;
};


void
RestServer::shutdown() {
    trigger_shutdown = true;
    auto duration = std::chrono::duration<float>(0.5);
    std::this_thread::sleep_for(duration);
}

std::unique_ptr<router::express_router_t<>>
RestServer::getRouter(){
    Logging::log()->trace("RestServer GetRouter being called");
    if (router == nullptr) {
        throw std::logic_error("Unable to get Router since its already out for route population or server has started");
    }
    return std::move(router);
};
void
RestServer::setRouter(std::unique_ptr<router::express_router_t<>> router_arg) {
    Logging::log()->trace("RestServer setRouter being called");
    if (router != nullptr) {
        throw std::logic_error("Unable to set Router since its already active. Check logic.");
    }
    router = std::move(router_arg);
}

void
RestServer::configureRoutes() {

    Logging::log()->trace("ReST Server configuring default routes");

    router->http_get(
            R"(/api/v1/ready)",
            [](auto req, auto params) {
                json j = "{}"_json;
                j["data"] = "{}"_json;
                j["response"]["status"] = 200;
                j["response"]["message"] = "Ready command received successfully";
                return req->create_response()
                        .append_header( restinio::http_field_t::access_control_allow_origin, "*" )
                        .set_body( j.dump())
                        .done();
            });

    //Routes::Get(router, "/api/v1/ready", Routes::bind(&RestServer::Ready, this));
    //Routes::Get(router, "/api/v1/auth", Routes::bind(&RestServer::Auth, this));

//    Routes::Get(router, "/api/v1/app/:thingBase", Routes::bind(&RestServer::GetThingManagers, this)); // securities, other tms

}

/*
void
RestServer::Auth(const Rest::Request &request, Http::ResponseWriter response) {
    // TODO - add basic auth here, then check in functions
    response.send(Http::Code::Ok);
}

*/
