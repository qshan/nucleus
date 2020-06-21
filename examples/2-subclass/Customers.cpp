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

#include "Customers.hpp"
#include "RestServer.hpp"

using namespace nucleus;
using namespace nucleus::examples::subclass;

using nlohmann::json;
using namespace pmem::obj;

Customer::Customer(const std::string& name_arg, const std::string& city_arg, int order_count_arg) :
    p_name(make_persistent<pmem::obj::string>(name_arg)),
    p_city(make_persistent<pmem::obj::string>(city_arg)),
    order_count(order_count_arg) {

    Logging::log()->debug("Customer Persistent Constructor called with {} {} {}",
                          p_name->c_str(), p_city->c_str(), order_count);

}

Customer::~Customer() {
    Logging::log()->debug("Customer Persistent Destructor called for {}", p_name->c_str());
}

Customers::Customers()
    : p_customer_list{make_persistent<pmem::obj::vector<Customer>>()}
{
    Logging::log()->debug("Customers Persistent Constructor called ");
}

Customers::~Customers()
{
    Logging::log()->debug("Customers Persistent Destructor called");
    auto pop = pmem::obj::pool_by_pptr(p_customer_list);
    pmem::obj::transaction::run(pop, [this] {
                delete_persistent<pmem::obj::vector<Customer>>(p_customer_list);
    });
}

void
Customers::Initialize()
{
    // Initialize any child objects here
    Logging::log()->trace("Customers is initializing");

    // Add sample customers
    auto pop = pmem::obj::pool_by_pptr(p_customer_list);
    pmem::obj::transaction::run(pop, [this] {

        p_customer_list->emplace_back("Peter", "Singapore", 26);
        p_customer_list->emplace_back("Elizabeth", "Australia", 13);
        p_customer_list->emplace_back("Mary", "Canada");

    });

}

void
Customers::Start(){
    Logging::log()->debug("Customers is starting");

    // App::init(this); RUNTIME App instance should be called here, if needed

    auto router = RestServerRouter::getRestServerRouter().getRouter();

    router->http_get(
        R"(/api/v1/app/customers)",
        [this](auto req, auto params) {

            json j = "{}"_json;
            j["data"]["customers"] = "[]"_json;

            for (auto &it : *p_customer_list) {
                json c = "{}"_json;
                c["name"] = it.p_name->c_str();
                c["city"] = it.p_city->c_str();
                c["order_count"] = (int) it.order_count;
                j["data"]["customers"].push_back(c);
            }

            return req->create_response()
                    .append_header(restinio::http_field_t::access_control_allow_origin, "*")
                    .set_body(j.dump())
                    .done();
        }
    );

    router->http_post(
        R"(/api/v1/app/customers)",
        [this](auto req, auto params) {

            Logging::log()->trace("HelloWorld adding new customer");

            auto j_req = json::parse(req->body());
            auto pop = pmem::obj::pool_by_pptr(p_customer_list);
            pmem::obj::transaction::run(pop, [this, &j_req] {
                p_customer_list->emplace_back(j_req["name"], j_req["city"], j_req["order_count"].template get<int>());
            });

            json j = "{}"_json;
            j["response"]["message"] = fmt::format("Customer updated");

            return req->create_response(restinio::status_created())
                    .append_header( restinio::http_field_t::access_control_allow_origin, "*" )
                    .set_body( j.dump())
                    .done();
        }
    );


    // TODO - Add other routes for getting single customer, adding new customers, deleting customers etc.

    // return the router to the RestServer
    RestServerRouter::getRestServerRouter().setRouter(std::move(router));

    // App::init(this); RUNTIME App instance should be called here, if needed

}

void
Customers::Stop()
{
    // if you create any volatile objects, delete them here
    Logging::log()->trace("Customers is stopping");

}