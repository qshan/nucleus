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

#ifndef MYAPP_H
#define MYAPP_H

#include "Platform.hpp"
#include "Customers.hpp"


class MyApp {

public:
    MyApp();            // this at pool creation or app reset. It does not run on each application start
    ~MyApp();           // this happens when the class instance is being deleted from the pool. It is not called on app close.

    void Initialize();  // this happens at object creation, typically to init downstream objects that rely on this obj
    void Start();       // this happens each time the applications runs
    void Stop();        // this happens when the app is shutting down. Note there is no runtime destructor!

    inline static const int layout_version = 0;

private:
    // These are the persistent memory objects for this class.
    pmem::obj::persistent_ptr<Customers> p_customers;

};

#endif //MYAPP_H
