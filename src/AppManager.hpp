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

#ifndef NUCLEUS_APPMANAGER_HPP
#define NUCLEUS_APPMANAGER_HPP

#include "Nucleus.hpp"
#include "PoolManager.hpp"
#include "RestServer.hpp"
#include "MyApp.hpp"

namespace nucleus {

    class PoolManager; // forward declaration
    struct rootStruct; // forward declaration

    class AppManager {
        // friend Nucleus;

    public:
        AppManager();
        ~AppManager();

        void Run();
        pmem::obj::persistent_ptr<MyApp> GetApp();
        void Exit(int s); // for Signal types - move back to private after setting up Nucleus class def

    private:
        pmem::obj::pool<NucleusPool> nucleus_pool;
        void SetAppState(AppState state);
        AppState GetAppState();
        std::string GetAppStateName();
        std::string GetAppStateName(AppState);
        void Exit(const std::string &reason);
    };


}

#endif //NUCLEUS_APPMANAGER_HPP
