//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//
#ifndef CUSTOMER_H
#define CUSTOMER_H
#include "Nucleus.hpp"

using namespace pmem::obj;
using namespace pmem::obj::experimental;

// This is the vector element - ie a single customer.
class Customer {
    // TODO - add functions here for creation of individual customer vars and managing individual customer objects
private:
    persistent_ptr<string> p_name;
    persistent_ptr<string> p_city;
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

private:
    // These are the persistent memory objects for this class
    persistent_ptr<pmem::obj::experimental::vector<Customer>> p_customers;
};

#endif //CUSTOMER_H
