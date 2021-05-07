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

#ifndef SUBCLASS_H
#define SUBCLASS_H

#include <RestServer.hpp>
#include "Platform.hpp"
#include "Segvec.hpp"
#include "Basebench.hpp" 

namespace nucleus::examples::subclass {

/**
 * Demonstrates including a persistent memory class into a persistent container
 */
class SubClass {

public:
    explicit SubClass(const CTX& ctx);                      // this at pool creation or app reset. It does not run on each application start
    ~SubClass();                                            // this happens when the class instance is being deleted from the pool. It is not called on app close.

    SubClass(const SubClass&)                = delete;      // Copy
    SubClass(SubClass&&)                     = delete;      // Move
    SubClass& operator= (const SubClass & ) = delete;       // Copy Assign
    SubClass& operator= (SubClass && )      = delete;       // Move assign

    void Initialize(const CTX& ctx);                        // this happens at object creation, typically to init downstream objects that rely on this obj
    void Start(const CTX& ctx);                             // this happens each time the applications runs
    void Stop();                                            // this happens when the app is shutting down. Note there is no runtime destructor!

    inline static const int layout_version = 0;

private:
    // These are the persistent memory objects for this class.
    pmem::obj::persistent_ptr<Segvecs> p_Segvecs;

    // Server Context
    pmem::obj::experimental::v<CTX> ctx;

};


}

#endif //SUBCLASS_H

