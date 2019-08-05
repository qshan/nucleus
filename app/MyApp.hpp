//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//
#ifndef MYAPP_H
#define MYAPP_H
#include "Nucleus.hpp"

using namespace pmem::obj;
using namespace pmem::obj::experimental;

class MyApp {

public:
    MyApp();            // this at pool creation or app reset. It does not run on each application start
    ~MyApp();           // this happens when the class instance is being deleted from the pool. It is not called on app close.

    void Initialize();  // this happens at object creation, typically to init downstream objects that rely on this obj
    void Start();       // this happens each time the applications runs
    void OnUpdate() {}; // this is called several times per second for regular processing

private:
    // These are the persistent memory objects for this application.
    persistent_ptr<string> p_message;
    p<int> p_update_count;

};

#endif //MYAPP_H
