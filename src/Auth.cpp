//
// Copyright (c) 2018-2020 AXOMEM PTE LTD. CONFIDENTIAL. All rights reserved.
//



#include "Auth.hpp"

nucleus::AuthResponse nucleus::Auth::authorize_user(std::string username, std::string password) {
    return nucleus::AuthResponse::ok;
}

nucleus::Auth::Auth(const CTX& ctx_arg) : ctx(ctx_arg) {
    ctx->log->debug("nucleus::Auth() is constructing.");
    ctx->log->warn("Using allow-all authorisation model. All requests will be approved as anonymous user");
}
