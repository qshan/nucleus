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

#ifndef Segvec_H
#define Segvec_H

#include <string>
#include <fstream> 
#include <sstream> 

#include "Platform.hpp"
#include "Basebench.hpp"
#include <RestServer.hpp>

namespace nucleus::examples::subclass {

/**
 * Manages a container of Segvec.
 * @note Persistent Class
 */
class Segvecs {

public:
    explicit Segvecs(const CTX& ctx);                   // this at pool creation or app reset. It does not run on each application start
    ~Segvecs();                                         // this happens when the class is being deleted from the pool. It is not called on app close.

    Segvecs(const Segvecs&)                = delete;    // Copy
    Segvecs(Segvecs&&)                     = delete;    // Move
    Segvecs& operator= ( const Segvecs & ) = delete;    // Copy Assign
    Segvecs& operator= ( Segvecs && )      = delete;    // Move assign

    void Initialize(const CTX& ctx);                    // this happens at object creation, typically to init downstream objects that rely on this obj
    void Start(const CTX& ctx);                         // this happens each time the applications runs
    void Stop();                                        // this happens when the app is shutting down. Note there is no runtime destructor!

private:
    // These are the persistent memory objects for this class
    pmem::obj::persistent_ptr<pmem::obj::segment_vector<pmem::obj::string>> p_wordlist {
        pmem::obj::make_persistent<pmem::obj::segment_vector<pmem::obj::string>>()
    };

    // Server Context
    pmem::obj::experimental::v<CTX> ctx;

};

}

#endif //Segvec_H
