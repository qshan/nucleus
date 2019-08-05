//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//

#include "PoolManager.hpp"
#include "MyApp.hpp"
#include "RestServer.hpp"
#include "json.hpp"
#include "fmt/format.h"

using namespace nucleus;
using nlohmann::json;


Customers::Customers()
    : p_customers{make_persistent<pmem::obj::experimental::vector<Customer>>()}
{
    Logging::log()->debug("Customer Persistent Constructor called ");
}

Customers::~Customers()
{
    Logging::log()->debug("CustomerPersistent Destructor called");
    pmem::obj::transaction::run(
            PoolManager::getPoolManager().getPoolForTransaction(), [&] {
                delete_persistent<pmem::obj::experimental::vector<Customer>>(p_customers);
            });
}

void
Customers::Initialize()
{
    // child objects->Initialize any child objects here;
    Logging::log()->trace("New Customer is initializing");

}

void
Customers::Start(){
    Logging::log()->debug("Customer is starting");

    // App::init(this); RUNTIME App instance should be called here, if needed

    auto router = RestServerRouter::getRestServerRouter().getRouter();

    router->http_get(
            R"(/api/v1/app/customers)",
            [&](auto req, auto params) {

                json j = "{}"_json;

                // TODO - iterate the customer vector, listing customers

                //j["data"]["value"] = p_message->c_str();
                j["response"]["status"] = 200;

                return req->create_response()
                        .append_header( restinio::http_field_t::access_control_allow_origin, "*" )
                        .set_body( j.dump())
                        .done();
            });

    // TODO - Add other routes for getting single customer, adding new customers, deleting customers etc.

    // return the router to the RestServer
    RestServerRouter::getRestServerRouter().setRouter(std::move(router));

    // App::init(this); RUNTIME App instance should be called here, if needed

}
