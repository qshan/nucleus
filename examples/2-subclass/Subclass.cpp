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

#include "Subclass.hpp"

using namespace nucleus;
using namespace nucleus::examples::subclass;
using namespace pmem::obj;

SubClass::SubClass(const CTX& ctx_arg) : p_customers{pmem::obj::make_persistent<Customers>(ctx_arg)}
{
    ctx_arg->log->debug("SubClass Persistent Constructor called. App name is {}", ctx_arg->config->app_name);
}

SubClass::~SubClass()
{
    ctx.get()->log->debug("SubClass Persistent Destructor called");
    auto pop = pmem::obj::pool_by_pptr(p_customers);
    pmem::obj::transaction::run(pop, [this] {
                delete_persistent<Customers>(p_customers);
    });
}

void
SubClass::Initialize(const CTX& ctx_arg)
{
    ctx = ctx_arg;
    // Initialize any child objects here
    ctx.get()->log->trace("SubClass is initializing");
    p_customers->Initialize(ctx);
}

void
SubClass::Start(const CTX& ctx_arg) {
    if (ctx.get() == nullptr) {
        ctx = ctx_arg;
    }

    ctx.get()->log->debug("SubClass is starting");

    p_customers->Start(ctx);
    // App::init(this); RUNTIME App instance should be called here, if needed

}

void
SubClass::Stop()
{
    // if you create any volatile objects, delete them here
    p_customers->Stop();
    ctx.get()->log->trace("SubClass is stopping");

}

RestServerRouter::router_ptr_t
SubClass::RegisterRestRoutes ( RestServerRouter::router_ptr_t router) {

    return p_customers->RegisterRestRoutes(std::move(router));

}