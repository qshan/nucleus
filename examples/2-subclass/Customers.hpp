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

#ifndef CUSTOMER_H
#define CUSTOMER_H
#include "Platform.hpp"
#include <string>

class Customers;

// This is the vector element - ie a single customer.
class Customer {

friend Customers;

public:
    explicit Customer(const std::string& name_arg = "", const std::string& city_arg = "", int order_count_arg = 0);
    ~Customer();
    // Customer(const Customer&)                = delete; // Copy is used by vector
    // Customer(Customer&&)                     = delete; // Move is used by vector
    // Customer& operator= ( const Customer & ) = delete; // Copy Assign kept otherwise copy implicitly deleted
    // Customer& operator= ( Customer && )      = delete; // Move assign kept otherwise copy implicitly deleted

private:
    pmem::obj::persistent_ptr<pmem::obj::string> p_name;
    pmem::obj::persistent_ptr<pmem::obj::string> p_city;
    pmem::obj::p<int> order_count;
};

// This is the container -ie vector of customer

class Customers {

public:
    Customers();            // this at pool creation or app reset. It does not run on each application start
    ~Customers();           // this happens when the class is being deleted from the pool. It is not called on app close.

    Customers(const Customers&)                = delete; // Copy
    Customers(Customers&&)                     = delete; // Move
    Customers& operator= ( const Customers & ) = delete; // Copy Assign
    Customers& operator= ( Customers && )      = delete; // Move assign

    // TODO - add functions here for managing customers. ReST interfaces should bind to these.

    void Initialize();  // this happens at object creation, typically to init downstream objects that rely on this obj
    void Start();       // this happens each time the applications runs
    void Stop();        // this happens when the app is shutting down. Note there is no runtime destructor!

private:
    // These are the persistent memory objects for this class
    pmem::obj::persistent_ptr<pmem::obj::vector<Customer>> p_customer_list;
};

#endif //CUSTOMER_H
