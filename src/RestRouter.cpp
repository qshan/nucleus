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

#include "RestRouter.hpp"
#include <regex>
#include <typeinfo>  // still needed for typeid
#include "Utilities.hpp"

using nlohmann::json;
using namespace restinio;


namespace nucleus {

RestRouter::RestRouter(Logging* logging) : log(logging->get_logger()) {

    log->trace("RestRouter is constructing");

}

RestRouter::~RestRouter() {
    log->trace("RestRouter::~() called");
}

void
RestRouter::add(restinio::http_method_id_t method, const std::string& route_path, route_callback_simple3_t callback_arg) {
    auto options = default_route_options();
    route_callback_t callback = [this, callback_arg](const auto& req, const auto& params, const auto& session, auto &resp){
        return callback_arg(req, params, resp);
    };
    add(method, route_path, options, callback);
}

void
RestRouter::add(restinio::http_method_id_t method, const std::string& route_path, route_callback_simple2_t callback_arg) {
    auto options = default_route_options();
    route_callback_t callback = [this, callback_arg](const auto& req, const auto& params, const auto& session, auto &resp){
        return callback_arg(req, resp);
    };
    add(method, route_path, options, callback);
}

void
RestRouter::add(restinio::http_method_id_t method,
                          const std::string& route_path,
                          route_callback_t callback) {
    auto options = default_route_options();
    add(method, route_path, options, callback);
}

void
RestRouter::add(restinio::http_method_id_t method,
                          const std::string& route_path,
                          route_options_t options,
                          route_callback_t callback) {

    log->debug("RestRouter adding route {} {}", method.c_str(),  route_path);

    link_route_options(method, route_path, options);

    router->add_handler(method, route_path,
                        [this, callback, route_path](const restinio::request_handle_t &req,
                                                const restinio::router::route_params_t &params) {

        return Request(req, params, callback);

    });

}

restinio::request_handling_status_t
RestRouter::Request(const restinio::request_handle_t &req,
                    const restinio::router::route_params_t &params,
                    route_callback_t callback) {

    std::chrono::time_point<std::chrono::high_resolution_clock> start;

    auto options = get_route_options(req, params);

    RequestStart(req, options, start);

    // Json response object
    nlohmann::json resp = "{}"_json;
    resp["response"] = "{}"_json;

    restinio::http_status_line_t status;

    session_t session = nullptr;

    try {

        session = get_session(req, options);

        log->trace("Calling PreCallback");
        RequestPreCallback(req, options, session);
        log->trace("Calling App-provided Callback");
        status = callback(req, params, session, resp);
        log->trace("Back from callback. Status is {}", status.status_code().raw_code());

    } catch ( const http_error &exc) {

        auto parsed_http_error = http_error_parse(exc.what());
        status = http_status_line_t{ http_status_code_t(std::get<0>(parsed_http_error)),
                                                std::get<1>(parsed_http_error) };

    } catch ( const std::exception &exc) {

        status = http_status_line_t{http_status_code_t(restinio::status_internal_server_error().status_code()),
                                    fmt::format( "({}) {}", typeid(exc).name(), exc.what())};

    }

    resp["response"]["status"] = fmt::format("{} {}", status.status_code().raw_code(), status.reason_phrase());

    RequestEnd(req, options, start, session, resp, status);

    // return the response;
    return req->create_response(status)
            .append_header( restinio::http_field_t::access_control_allow_origin, "*" )
            .append_header(restinio::http_field_t::content_type, "application/json")
            .set_body( resp.dump())
            .done();

}

void RestRouter::RequestStart(const restinio::request_handle_t &req,
                              const route_options_t& options,
                              std::chrono::time_point<std::chrono::high_resolution_clock>& start) {

        start = std::chrono::high_resolution_clock::now();

        if (log->level() == spdlog::level::trace) {

            // Log headers for tracing
            std::stringstream headers;
            for(const auto & it : req->header()) {headers << it.name() << ":" << it.value() << "|";}

            auto body = strleft_utf8(req->body(),200,  " (truncated in log...)");
            body.erase(std::remove(body.begin(), body.end(), '\n'), body.end());

            log->trace("RESTAPI START: {} || {} || {}", req->remote_endpoint(), headers.str(), body);
        }
}

session_t
RestRouter::get_session(const req_t& req, const route_options_t& options)  {

    return &anonymous_session;
}

void
RestRouter::RequestEnd(const restinio::request_handle_t &req,
                       const route_options_t& options,
                       const std::chrono::time_point<std::chrono::high_resolution_clock>& start,
                       session_t session,
                       const json& resp,
                       const restinio::http_status_line_t& status) {

    auto path = (std::string) req->header().path();

    auto log_level = status.status_code() == restinio::status_internal_server_error().status_code() ?
                                                 spdlog::level::err : options->log_level;

    // Calculate response time
    std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
    auto elapsed_ms = (long) std::round(elapsed.count() * 1000);

    // Log request
    // This is roughly Common Log Format with extra field for reason phrase. Eventually to separate into http log
    std::string username = session == nullptr ? "unknown" : session->username;
    log->log(log_level, "{} {} {} {}?{} {} {} {}", req->remote_endpoint(), username,
                  req->header().method(), path, req->header().query(),
                  status.status_code().raw_code(), elapsed_ms, status.reason_phrase());

}


void
RestRouter::link_route_options(const restinio::http_method_id_t& method, const std::string& route_path, route_options_t options_arg) {
    route_option_map[fmt::format("{}__{}", method.c_str(), route_path)] = options_arg->option_set_id;
}

route_options_t
RestRouter::make_route_options(bool custom) {

    if (route_options.empty()) {
        // this is the default option
        route_options.emplace(std::make_pair(0, RouteOptions()));
    }

    if (custom) {
        auto id = route_options.size();
        auto r = route_options.emplace(std::make_pair(id, RouteOptions()));

        assert(std::get<1>(r) == true && "Insert should not fail unless running multi threaded."
                                         " Should be single threaded at route registration time");

        auto it = std::get<0>(r);
        it->second.option_set_id = it->first;

        return &it->second;
    } else {

        return &route_options.at(0);
    }
}

route_options_t
RestRouter::custom_route_options() {
    return make_route_options(true);
}

route_options_t
RestRouter::default_route_options() {
    return make_route_options(false);
}

route_options_t
RestRouter::get_route_options(const req_t& req, const params_t& params) {

    auto option_id = route_option_map[fmt::format("{}__{}", req->header().method(), params.match())];

    auto i = route_options.find(option_id);
    if(i != route_options.end()) {
        return &i->second;
    } else {
        return default_route_options();
    }

}

void
RestRouter::RequestPreCallback(const restinio::request_handle_t &req, const route_options_t &options,
                              session_t session) {
    // For adding upstream functions
}

std::tuple<std::uint16_t, std::string>
RestRouter::http_error_parse(const std::string& message_arg) {
    std::uint16_t code = 500;
    std::string message;
    std::string code_str;

    std::size_t space_pos = message_arg.find(' ');
    if (space_pos!=std::string::npos)  {
        code_str = message_arg.substr(0, space_pos);
        message = message_arg.size() >= space_pos+1 ? message_arg.substr(space_pos+1) : "";
    } else {
        code_str = message_arg;
    }

    char *end;
    code = (std::uint16_t) strtol( code_str.c_str(), &end, 10 );
    if (*end) {
        code = 500;
        message = "(invalid status_code) " + message;
    }
    return {code, message};

}

void RestRouter::non_matched_request_handler(restinio::router::non_matched_request_handler_t handler) {
    router->non_matched_request_handler(std::move(handler));

}


}