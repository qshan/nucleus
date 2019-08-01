//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//

#ifndef AXONCORE_UTILS_HPP
#define AXONCORE_UTILS_HPP

#include <cstdint>
#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>

#ifndef _WIN32
// Linux specifics

#include <unistd.h>
#include <sys/stat.h>
#include <vector>

#define CREATE_MODE_RW (S_IWUSR | S_IRUSR)

// check if file exists

bool file_exists(const std::string& filename);
void configure_logging();
std::vector<std::string> split(std::string str, std::string token);

#else

#include <corecrt_io.h>
#include <process.h>
#include <windows.h>

#define CREATE_MODE_RW (S_IWRITE | S_IREAD)

// Win - check file exists
static int file_exists(char const *file)
{
	return _access(file, 0);
}

#endif

// The following functions help make ENUMs with friendly names

#define MAKE_ENUM(VAR) VAR,
#define MAKE_STRINGS(VAR) #VAR,
#define MAKE_ENUM_AND_STRINGS(source, enumName, enumStringName) \
    enum enumName { \
    source(MAKE_ENUM) \
    };\
const char* const enumStringName[] = { \
    source(MAKE_STRINGS) \
    };

#endif //AXONCORE_UTILS_HPP
