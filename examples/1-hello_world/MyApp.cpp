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

#include "PoolManager.hpp"
#include "MyApp.hpp"
#include "RestServer.hpp"
#include "json.hpp"
#include "fmt/format.h"

using namespace nucleus;
using nlohmann::json;


MyApp::MyApp()
    : p_message{make_persistent<pmem::obj::string>("Hello World")}
    , p_update_count{0}
{
    Logging::log()->debug("MyApp Persistent Constructor called");
}

MyApp::~MyApp()
{
    Logging::log()->debug("MyApp Persistent Destructor called");
    auto pop = pmem::obj::pool_by_pptr(p_message);
    pmem::obj::transaction::run(pop, [&] {
                delete_persistent<pmem::obj::string>(p_message);
            });
}

void
MyApp::Initialize()
{
    // child objects->Initialize any child objects here;
    Logging::log()->trace("MyApp is initializing");

}

void
MyApp::Start(){
    Logging::log()->debug("MyApp is starting");

    // Map the APIS

    auto router = RestServerRouter::getRestServerRouter().getRouter();

    router->http_get(
            R"(/api/v1/app/message)",
            [&](auto req, auto params) {

                json j = "{}"_json;
                j["data"]["value"] = p_message->c_str();

                return req->create_response()
                        .append_header( restinio::http_field_t::access_control_allow_origin, "*" )
                        .set_body( j.dump())
                        .done();
            });

    router->http_put(
            R"(/api/v1/app/message)",
            [&](auto req, auto params) {

                auto j_req = json::parse(req->body());
                std::string message_value = j_req["value"];
                Logging::log()->trace("MyApp Message is being set to {}.", message_value);
                auto pop = pmem::obj::pool_by_pptr(p_message);
                pmem::obj::transaction::run(pop, [&] {
                    p_message->assign(message_value);
                    p_update_count++;
                });

                json j = "{}"_json;
                j["response"]["message"] = fmt::format("Message value updated {} time(s) so far", p_update_count);

                return req->create_response()
                        .append_header( restinio::http_field_t::access_control_allow_origin, "*" )
                        .set_body( j.dump())
                        .done();
            });

    // return the router to the RestServer
    RestServerRouter::getRestServerRouter().setRouter(std::move(router));

    // App::init(this); RUNTIME App instance should be called here, if needed

}

void
MyApp::Stop()
{
    // if you create any volatile objects, delete them here;
    Logging::log()->trace("MyApp is stopping");

}
