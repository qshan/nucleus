//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//

#ifndef NUCLEUS_APPMANAGER_HPP
#define NUCLEUS_APPMANAGER_HPP

#include "Nucleus.hpp"
#include "RestServer.hpp"

class MyApp;

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
        pmem::obj::pool<rootStruct> pool_root;
        void SetAppState(AppState state);
        AppState GetAppState();
        std::string GetAppStateName();
        std::string GetAppStateName(AppState);
        void Update() {};
        void Exit(const std::string &reason);
    };


}

#endif //NUCLEUS_APPMANAGER_HPP
