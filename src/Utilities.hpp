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

#ifndef NUCLEUS_UTILS_HPP
#define NUCLEUS_UTILS_HPP

#include <string>
#include <vector>


std::string strleft_utf8(const std::string &str, size_t max_size, const std::string& continuation = "");

// The following functions help make ENUMs with friendly names
// @copyright https://stackoverflow.com/a/52264973
// Note - the comma in the lists comes from the comma at the end of MAKE_ENUM/MAKE_STRINGS

#define MAKE_ENUM(VAR) VAR,
#define MAKE_STRINGS(VAR) #VAR,
#define MAKE_ENUM_AND_STRINGS(source, enumName, enumStringName) \
    enum class enumName { \
    source(MAKE_ENUM) \
    };\
const char* const enumStringName[] = { \
    source(MAKE_STRINGS) \
    };


std::vector<std::string> split(const std::string& str, const std::string& token);

#endif //NUCLEUS_UTILS_HPP
