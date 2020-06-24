//
// Copyright (c) 2018-2020 AXOMEM PTE LTD. CONFIDENTIAL. All rights reserved.
//



#ifndef NUCLEUS_AUTH_H
#define NUCLEUS_AUTH_H

#include "Platform.hpp"
#include "RestServer.hpp"

namespace nucleus {

    enum class AuthResponse {ok, not_authenticated, not_authorized, request_error, server_error};
    using basic_auth_t = std::tuple<restinio::http_status_line_t, std::string>;


class Auth {

public:
    Auth(const CTX &ctx);

    virtual AuthResponse authorize_user(std::string username, std::string password);

private:
    CTX ctx;

};

}
#endif //NUCLEUS_AUTH_H
