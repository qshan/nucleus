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

namespace nucleus::examples::subclass {


SubClass::SubClass(const CTX& ctx_arg) : p_Segvecs{pmem::obj::make_persistent<Segvecs>(ctx_arg)} {

    ctx_arg->log->debug("SubClass Persistent Constructor called. App name is {}", ctx_arg->config->app_name);

}

SubClass::~SubClass() {

    ctx.get()->log->debug("SubClass Persistent Destructor called");

    auto pop = pmem::obj::pool_by_pptr(p_Segvecs);
    pmem::obj::transaction::run(pop, [this] {
        pmem::obj::delete_persistent<Segvecs>(p_Segvecs);
    });

}

void
SubClass::Initialize(const CTX& ctx_arg) {

    ctx = ctx_arg;
    ctx.get()->log->trace("SubClass is initializing");

    // Initialize any child objects here
    p_Segvecs->Initialize(ctx);

}

void
SubClass::Start(const CTX& ctx_arg) {

    if (ctx.get() == nullptr) {
        ctx = ctx_arg;
    }

    ctx.get()->log->debug("SubClass is starting");

    p_Segvecs->Start(ctx);
    // App::init(this); RUNTIME App instance should be called here, if needed

}

void
SubClass::Stop() {

    // if you create any volatile objects, delete them here
    p_Segvecs->Stop();
    ctx.get()->log->trace("SubClass is stopping");

}


}
