//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//

#ifndef NUCLEUS_LOGGING_H
#define NUCLEUS_LOGGING_H

#include "spdlog/spdlog.h"

class Logging {

public:
    ~Logging();
    static Logging *init();
    static std::shared_ptr<spdlog::logger> &log();

private:
    explicit Logging(const std::string &name); // hide constructor - use init for singleton
    static Logging *logger;

    static std::shared_ptr<spdlog::logger> mylog;  // this is the

    Logging(const Logging &);  // hide copy constructor
    Logging &operator=(const Logging &); // hide assignment operator

};

#endif //NUCLEUS_LOGGING_H
