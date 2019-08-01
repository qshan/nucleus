//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//

#ifndef AXONCORE_REST_HPP
#define AXONCORE_REST_HPP

#include "PoolManager.hpp"
#include "Nucleus.hpp"
#include "restinio/all.hpp"

using namespace nucleus;
using namespace restinio;

    class RestServer {
    public:
        explicit RestServer();
        static RestServer *getRestServer(); // for Singleton
        void run(); // should be run on own thread
        void shutdown(); // trigger for shutdown... join thread afterwards
        std::unique_ptr<router::express_router_t<>> getRouter();
        void setRouter(std::unique_ptr<router::express_router_t<>>);
    private:
        bool trigger_shutdown;
        std::unique_ptr<router::express_router_t<>> router;
        static RestServer *restEndpoint; // for Singleton
        void configureRoutes();
    };


#endif //AXONCORE_REST_HPP
