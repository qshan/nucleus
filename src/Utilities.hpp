//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//

#ifndef NUCLEUS_UTILS_HPP
#define NUCLEUS_UTILS_HPP

// The following functions help make ENUMs with friendly names
// Source: https://stackoverflow.com/a/52264973

#define MAKE_ENUM(VAR) VAR,
#define MAKE_STRINGS(VAR) #VAR,
#define MAKE_ENUM_AND_STRINGS(source, enumName, enumStringName) \
    enum enumName { \
    source(MAKE_ENUM) \
    };\
const char* const enumStringName[] = { \
    source(MAKE_STRINGS) \
    };

#endif //NUCLEUS_UTILS_HPP
