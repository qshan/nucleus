//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//

#ifndef NUCLEUS_APPMANAGER_HPP
#define NUCLEUS_APPMANAGER_HPP

#define APPSTATE_ENUM(APS) APS(NEW) APS(INITIALIZING) APS(STARTING) APS(RUNNING) APS(PAUSED) APS(EXITING) \
                           APS(STOPPED) APS(MAINTENANCE) APS(UNRECOVERABLE)

#define ASIO_HEADER_ONLY 1
#define ASIO_STANDALONE 1
#include "restinio/all.hpp"
#include "Nucleus.hpp"

namespace nucleus {

    class PoolManager; // forward declaration

    MAKE_ENUM_AND_STRINGS(APPSTATE_ENUM, AppState, AppStateNames)

    class AppManager {
        friend PoolManager;

    public:
        AppManager();
        ~AppManager();

        void Run();

        pmem::obj::persistent_ptr<MyApp> GetApp();

    private:
        pmem::obj::persistent_ptr<MyApp> thingbase1;
        void SetAppState(AppState state);
        AppState GetAppState();
        std::string GetAppStateName();
        std::string GetAppStateName(AppState);
        pmem::obj::p<AppState > appState;
        void resetApp();
        void Update() {};
        void Exit(int s); // for Signal types
        void Exit(const std::string &reason);
    };


}

#endif //NUCLEUS_APPMANAGER_HPP
