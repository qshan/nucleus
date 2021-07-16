# Supporting libraries

We are grateful for the contributions authors have made to providing these libraries as Open Source:

| Library | Source | Release* | Commit | Upd* |
| ------- | ------- | ------ | -------| -------|
RESTINIO | https://github.com/Stiffstream/restinio | v.0.6.13 | 0052518f5692f8f051031e06d933b726191be97e |*| 
HTTP_PARSER | https://github.com/nodejs/http-parser | 2.9.4 | 2343fd6b5214b2ded2cdcf76de2bf60903bb90cd |-|
ASIO | https://github.com/chriskohlhoff/asio | 1.19.1 | 4c5a7cca62cb4354de08298e868bea54bfd6692b |*|
JSON.HPP | https://github.com/nlohmann/json | v3.9.1 | db78ac1d7716f56fc9f1b030b715f872f93964e4 |-|
SPDLOG | https://github.com/gabime/spdlog |  v1.8.5 | 100f30043f33277122e0991c83845a2617172ffd |*| 
FMT | https://github.com/fmtlib/fmt | 7.1.3 | 7bdf0628b1276379886c7f6dda2cef2b3b374f0b |*|

\* In Release, \>\> indicates a later commit was taken than the release to resolve issues or use latest at sync

\* In Upd \>\> indicates a version update in current release

## Important
These libraries are subject to separate copyrights and are not included in the default copyright notice
at the root of the source tree. They are included here for your convenience. However, you are responsible for
ensuring ongoing compliance with the terms of these copyright owners.

## Update notes

### General update process
1. Git clone / pull the required repo
2. Check dependencies and checkout the required tag
3. Delete the /ext/packagename/* contents in our repo
4. Paste in the contents per below  

### Restinio
1. Per general update process above
2. Copy the /dev/restinio directory into /ext/restinio
 
### HTTP_Parser
(Update next release)
1. Obtain the package via releases https://github.com/nodejs/http-parser/releases 
2. Delete the etc/http_parser dir
3. Extract the contents of zip to above directory

### ASIO
1. Per general update process above
2. Copy the `asio.hpp` and `asio` directory from /asio/include directory

Note: On Fedora, ASIO is packaged with Boost. Avoiding using OS package managers for now.

### JSON.HPP
1. Per general update process above
2. Copy the `single_include/nlohhman/json/json.hpp` file to `ext/json.hpp`

### SPDLOG
1. Per general update process above
2. Copy the include/spdlog directory into ext/spdlog

### FMT
1. Per general update process above
2. extract include/fmt into ext/fmt
Note: There are still issues with spdlog with v8 of fmt. https://github.com/gabime/spdlog/issues/1975
   
   