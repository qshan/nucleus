//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//

#ifndef AXONCORE_REST_HPP
#define AXONCORE_REST_HPP

#define ASIO_HEADER_ONLY 1
#define ASIO_STANDALONE 1

#include "PoolManager.hpp"
#include "Nucleus.hpp"
#include "restinio/all.hpp"

using namespace nucleus;
using namespace restinio;

    class RestServerRouter {

    public:
        explicit RestServerRouter();
        std::unique_ptr<router::express_router_t<>> getRouter();
        void setRouter(std::unique_ptr<router::express_router_t<>>);
        static RestServerRouter & getRestServerRouter();
    private:
        std::unique_ptr<router::express_router_t<>> router;
    };

    class RestServer {

    public:
        explicit RestServer( std::unique_ptr<router::express_router_t<>> router);
        void shutdown(); // will block until server is shut down

    private:
        using my_traits_t = restinio::traits_t<
                restinio::asio_timer_manager_t,
                restinio::null_logger_t,
                router::express_router_t<>>;
        using my_server_t = restinio::http_server_t<my_traits_t>;
        my_server_t my_server;
        std::thread restinio_control_thread;
    };


#endif //AXONCORE_REST_HPP
