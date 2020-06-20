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

#include "MyApp.hpp"
#include "RestServer.hpp"

using namespace nucleus;
using namespace pmem::obj;

MyApp::MyApp()
{
    Logging::log()->debug("MyApp Persistent Constructor called ");
}

MyApp::~MyApp()
{
    Logging::log()->debug("MyApp Persistent Destructor called");
    auto pop = pmem::obj::pool_by_pptr(p_customers);
    pmem::obj::transaction::run(pop, [this] {
                delete_persistent<Customers>(p_customers);
            });
}

void
MyApp::Initialize()
{
    // Initialize any child objects here
    Logging::log()->trace("MyApp is initializing");
    p_customers->Initialize();
}

void
MyApp::Start(){
    Logging::log()->debug("MyApp is starting");

    p_customers->Start();
    // App::init(this); RUNTIME App instance should be called here, if needed

}

void
MyApp::Stop()
{
    // if you create any volatile objects, delete them here
    p_customers->Stop();
    Logging::log()->trace("MyApp is stopping");

}