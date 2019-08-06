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


MyApp::MyApp()
    : p_customers{make_persistent<Customers>()}
{
    Logging::log()->debug("MyApp Persistent Constructor called ");
}

MyApp::~MyApp()
{
    Logging::log()->debug("MyApp Persistent Destructor called");
    pmem::obj::transaction::run(
            PoolManager::getPoolManager().getPoolForTransaction(), [&] {
                delete_persistent<Customers>(p_customers);
            });
}

void
MyApp::Initialize()
{
    // child objects->Initialize any child objects here;
    Logging::log()->trace("MyApp is initializing");
    p_customers->Initialize();
}

void
MyApp::Start(){
    Logging::log()->debug("MyApp is starting");

    p_customers->Start();
    // App::init(this); RUNTIME App instance should be called here, if needed

}

