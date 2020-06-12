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

#ifndef CUSTOMER_H
#define CUSTOMER_H
#include "Nucleus.hpp"

using namespace pmem::obj;

class Customers;

// This is the vector element - ie a single customer.
class Customer {

friend Customers;

public:
    explicit Customer(const std::string& name_arg = "", const std::string& city_arg = "", int order_count_arg = 0);
    ~Customer();

private:
    persistent_ptr<pmem::obj::string> p_name;
    persistent_ptr<pmem::obj::string> p_city;
    p<int> order_count;
};

// This is the container -ie vector of customer

class Customers {

public:
    Customers();            // this at pool creation or app reset. It does not run on each application start
    ~Customers();           // this happens when the class is being deleted from the pool. It is not called on app close.

    // TODO - add functions here for managing customers. ReST interfaces should bind to these.

    void Initialize();  // this happens at object creation, typically to init downstream objects that rely on this obj
    void Start();       // this happens each time the applications runs
    void Stop();        // this happens when the app is shutting down. Note there is no runtime destructor!

private:
    // These are the persistent memory objects for this class
    persistent_ptr<pmem::obj::vector<Customer>> p_customer_list;
};

#endif //CUSTOMER_H
