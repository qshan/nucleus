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

#ifndef NUCLEUS_AUTH_H
#define NUCLEUS_AUTH_H

#include "Platform.hpp"
#include "RestServer.hpp"

namespace nucleus {

    enum class AuthResponse {ok, not_authenticated, not_authorized, request_error, server_error};
    using basic_auth_t = std::tuple<restinio::http_status_line_t, std::string>;


class Auth {

public:
    explicit Auth(const CTX &ctx);
    virtual ~Auth() = default;

    virtual AuthResponse authorize_user(const std::string& username, const std::string& password);

private:
    CTX ctx;

};

}
#endif //NUCLEUS_AUTH_H
