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

#include "Segvec.hpp"

namespace nucleus::examples::subclass {

Segvecs::Segvecs(const CTX& ctx_arg) {
    ctx_arg->log->debug("Segvecs Persistent Constructor called");
}

Segvecs::~Segvecs() {

    ctx.get()->log->debug("Segvecs Persistent Destructor called");

    auto pop = pmem::obj::pool_by_pptr(p_wordlist);
    pmem::obj::transaction::run(pop, [this] {
        pmem::obj::delete_persistent<pmem::obj::segment_vector<pmem::obj::string>>(p_wordlist);
    });

}

void
Segvecs::Initialize(const CTX& ctx_arg) {

    ctx = ctx_arg; 

    ctx.get()->log->info("READ WORD LIST INTO SEGMENT VECTOR"); 
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now(); 

    auto pop = pmem::obj::pool_by_pptr(p_wordlist);
    pmem::obj::transaction::run(pop, [this] {

        std::string line; 
        std::ifstream infile(filename); 

        while (std::getline(infile, line)) { 
            //auto temp = make_persistent<pmem::obj::string>(line); 
            p_wordlist->emplace_back(line); 
        }

    });

    std::chrono::steady_clock::time_point stop  = std::chrono::steady_clock::now();
    std::stringstream ss;
    ss << "TIME TAKEN = " << std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count() << " ms."; 
    ctx.get()->log->info(ss.str()); 
}

void
Segvecs::Start(const CTX& ctx_arg) {

    if (ctx.get() == nullptr) {
        ctx = ctx_arg;
    }

    ctx.get()->log->debug("Segvecs is starting");

    // App::init(this); RUNTIME App instance should be called here, if needed

    ctx.get()->rest_server->routes()->add(http_method_get(), R"(/api/v1/app/Segvecs/Binsearch/:query)",
        [this](const auto &req, const auto& params, auto &resp) {

        ctx.get()->log->info("BINARY SEARCH WORD LIST");
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        std::string query = restinio::cast_to<std::string>(params["query"]);

        long long Res = -1;
        auto it = std::lower_bound(p_wordlist->begin(), p_wordlist->end(), query);
        if (it == p_wordlist->end() || *it != query) {
            Res = -1;
        } else {
            Res = std::distance(p_wordlist->begin(), it);
        }

        resp["data"]["query_index"] = std::to_string(Res);

        std::chrono::steady_clock::time_point stop  = std::chrono::steady_clock::now();
        std::stringstream ss;
        ss << "TIME TAKEN = " << std::chrono::duration_cast<std::chrono::microseconds>(stop-start).count() << " us.";
        ctx.get()->log->info(ss.str());

        return status_ok();

    });

    ctx.get()->rest_server->routes()->add(http_method_get(), R"(/api/v1/app/Segvecs/Seqsearch/:query)",
        [this](const auto &req, const auto& params, auto &resp) {

        ctx.get()->log->info("SEQUENTIAL SEARCH WORD LIST");
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        std::string query = restinio::cast_to<std::string>(params["query"]);

        long long Res = -1;
        for (auto it = p_wordlist->begin(); it != p_wordlist->end(); ++it) {
            if (*it == query) {
                Res = std::distance(p_wordlist->begin(), it);
                break;
            }
        }

        resp["data"]["query_index"] = std::to_string(Res);

        std::chrono::steady_clock::time_point stop  = std::chrono::steady_clock::now();
        std::stringstream ss;
        ss << "TIME TAKEN = " << std::chrono::duration_cast<std::chrono::microseconds>(stop-start).count() << " us.";
        ctx.get()->log->info(ss.str());

        return status_ok();

        });

    ctx.get()->rest_server->routes()->add(http_method_post(), R"(/api/v1/app/Segvecs/addentry)",
            [this](const auto &req, auto &resp) {

        ctx.get()->log->trace("Segvecs adding new entry.");

        auto j_req = json::parse(req->body());
        std::string entrynew = restinio::cast_to<std::string>(j_req["entry"]);

        if (j_req["entry"].template get<std::string>().empty()) {
            throw http_error(status_bad_request(), "New entry empty! =(");
        }

        auto pop = pmem::obj::pool_by_pptr(p_wordlist);
        pmem::obj::transaction::run(pop, [this, &j_req] {

            std::string temp = restinio::cast_to<std::string>(j_req["entry"]);
            ctx.get()->log->trace(temp);
            p_wordlist->emplace_back(temp);

        });

        std::string resmsg = "Segvec updated with "+entrynew+"! =)";
        resp["response"]["message"] = fmt::format(resmsg);

        return status_created();

    });

}

void
Segvecs::Stop() {

    // if you create any volatile objects, delete them here
    ctx.get()->log->trace("Segvecs is stopping.");

}

}

