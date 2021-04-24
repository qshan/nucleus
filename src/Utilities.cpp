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

#include <stdexcept>
#include "Utilities.hpp"

/// @brief truncate utf8 string at a safe place
/// @copyright https://stackoverflow.com/questions/35328529/stdstring-optimal-way-to-truncate-utf-8-at-safe-place
/// @note Continuation sequence is appended to truncated length. So max is max_size + len(continuation)
std::string
strleft_utf8(const std::string &str, size_t max_size, const std::string& continuation)
{

    if (str.size() <= max_size) {
        return str;
    }

    for(size_t pos = max_size; pos > 0; --pos)
    {
        auto byte = static_cast<unsigned char>(str[pos]);
        if((byte & 0xC0u) != 0x80)
            return str.substr(0,pos) + continuation;
    }

    auto byte = static_cast<unsigned char>(str[0]);
    if((byte & 0xC0u) != 0x80)
        return (std::string) "";

    // The first byte isn't even a valid UTF-8 starting point
    throw std::invalid_argument("Malformed utf8 encoded text passed to strleft_utf8");
}

/// @copyright https://github.com/KjellKod/StringFix (Unlicense)
std::vector<std::string>
split(const std::string& stringToSplit, const std::string& delimiters) {

    // Skip all the text until the first delimiter is found
    std::string::size_type start = stringToSplit.find_first_not_of(delimiters, 0);
    std::string::size_type stop = stringToSplit.find_first_of(delimiters, start);

    std::vector<std::string> tokens;
    while (std::string::npos != stop || std::string::npos != start) {
        tokens.push_back(stringToSplit.substr(start, stop - start));
        start = stringToSplit.find_first_not_of(delimiters, stop);
        stop = stringToSplit.find_first_of(delimiters, start);
    }
    return tokens;
}