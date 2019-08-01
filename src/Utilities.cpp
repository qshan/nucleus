//
// Copyright (c) 2018-2019 AxoMem Pte Ltd.  All rights reserved.
//

#include "Utilities.hpp"
#include <vector>

bool file_exists(const std::string& filename) {
    struct stat buffer = {0};
    auto test = (bool)(stat (filename.c_str(), &buffer) == 0);
    // std::clog << "File check for " << filename << " is " << test << std::endl;
    return test;
    // pmdk source?
}


std::vector<std::string> split(std::string str, std::string token){
    std::vector<std::string>result;
    while(!str.empty()){
        auto index = str.find(token);
        if(index!=std::string::npos){
            result.push_back(str.substr(0,index));
            str = str.substr(index+token.size());
            if(str.empty())result.push_back(str);
        } else {
            result.push_back(str);
            str = "";
        }
    }
    return result;
    // https://stackoverflow.com/questions/41425569/checking-for-empty-string-in-c-alternatives
}
