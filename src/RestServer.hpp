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

// *****************************************************************
// For help with Restinio, see https://stiffstream.com/en/docs.html
// *****************************************************************

#ifndef NUCLEUS_REST_HPP
#define NUCLEUS_REST_HPP

#define ASIO_HEADER_ONLY 1
#define ASIO_STANDALONE 1

#include "Platform.hpp"
#include "restinio/all.hpp"

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
        ~RestServer();

    private:
        using my_traits_t = restinio::traits_t<
                restinio::asio_timer_manager_t,
                restinio::null_logger_t,
                router::express_router_t<>>;
        using my_server_t = restinio::http_server_t<my_traits_t>;
        my_server_t my_server;
        std::thread restinio_control_thread;
    };


#endif //NUCLEUS_REST_HPP
