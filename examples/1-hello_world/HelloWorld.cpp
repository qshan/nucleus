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


#include "HelloWorld.hpp"
#include "RestServer.hpp"

namespace nucleus::examples::helloworld {


HelloWorld::HelloWorld(const CTX& ctx) {

    ctx->log->debug("HelloWorld Persistent Constructor called. App name is {}", ctx->config->app_name);

}

HelloWorld::~HelloWorld() {

    ctx.get()->log->debug("HelloWorld Persistent Destructor called");
    auto pop = pmem::obj::pool_by_pptr(p_message);
    pmem::obj::transaction::run(pop, [this] {
                pmem::obj::delete_persistent<pmem::obj::string>(p_message);
    });

}

void
HelloWorld::Initialize(const CTX& ctx_arg) {

    ctx = ctx_arg;
    // Initialize any child objects here
    ctx.get()->log->trace("HelloWorld is initializing");

}

void
HelloWorld::Start(const CTX& ctx_arg) {

    if (ctx.get() == nullptr) {
        ctx = ctx_arg;
    }
    ctx.get()->log->debug("HelloWorld is starting");

    ctx.get()->rest_server->routes()->add(http_method_get(), R"(/api/v1/app/message)",
            [this](const auto &req, auto &resp) {

        resp["data"]["value"] = p_message->c_str();
        return status_ok();

    });

    ctx_arg->rest_server->routes()->add(http_method_put(), R"(/api/v1/app/message)",
            [this](const auto& req, auto &resp) {

        auto j_req = json::parse(req->body());

        std::string message_value = j_req["value"];

        if (message_value.empty()) {
            throw http_error(status_bad_request(), "Value must be provided");
        }

        auto pop = pmem::obj::pool_by_pptr(p_message);
        pmem::obj::transaction::run(pop, [this, &message_value] {
            p_message->assign(message_value);
            p_update_count++;
        });

        resp["response"]["message"] = fmt::format("Message value updated {} time(s) so far", p_update_count);

        return status_ok();
    });

}

void
HelloWorld::Stop() {

    // if you create any volatile objects, delete them here
    ctx.get()->log->trace("HelloWorld is stopping");

}


}