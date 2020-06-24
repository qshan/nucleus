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

#include "Auth.hpp"

nucleus::Auth::Auth(const CTX& ctx_arg) : ctx(ctx_arg) {
    ctx->log->debug("nucleus::Auth() is constructing.");
    ctx->log->warn("Using allow-all authorisation model. All requests will be approved as anonymous user");
}

nucleus::AuthResponse nucleus::Auth::authorize_user(const std::string& username, const std::string& password) {
    return nucleus::AuthResponse::ok;
}
