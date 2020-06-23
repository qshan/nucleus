# Supporting libraries

We are grateful for the contributions authors have made to providing these libraries as Open Source:

| Library | Source | Release* | Commit | Upd |
| ------- | ------- | ------ | -------| -------|
RESTINIO | https://github.com/Stiffstream/restinio | v.0.6.8 | c34f83a68e60537eba52e2d076ed22495ad4c5df |*| 
HTTP_PARSER | https://github.com/nodejs/http-parser | 2.9.4 | 2343fd6b5214b2ded2cdcf76de2bf60903bb90cd |*|
ASIO | https://think-async.com/Asio/ | 1.12.2 | N/A ||
JSON.HPP | https://github.com/nlohmann/json | v3.8.0 | e7452d87783fbf6e9d320d515675e26dfd1271c5 |*|
SPDLOG | https://github.com/gabime/spdlog |  v1.6.1 | 22a169bc319ac06948e7ee0be6b9b0ac81386604 |*| 
FMT | https://github.com/fmtlib/fmt | 6.2.1 | 19bd751020a1f3c3363b2eb67a039852f139a8d3 |*| 

\* In Release, \>\> indicates a later commit was taken than the release to resolve issues or use latest at sync

\* In Upd \>\> indicates a version update in current release

## Important
These libraries are subject to separate copyrights and are not included in the default copyright notice
at the root of the source tree. They are included here for your convenience. However, you are responsible for
ensuring ongoing compliance with the terms of these copyright owners.

## Update notes

### Restinio
1. Obtain the package via the releases page https://github.com/Stiffstream/restinio/releases
2. Delete the /etc/restinio/* contents in our repo 
2. Extract and copy the /dev/restinio directory into /ext/restinio
 
### HTTP_Parser
1. Obtain the package via releases https://github.com/nodejs/http-parser/releases
2. Delete the etc/http_parser dir
3. Extract the contents of zip to above director

### ASIO
Get the package and copy... #TBD

### JSON.HPP
1. Obtain from https://github.com/nlohmann/json/releases
2. Copy the `single_include/nlohhman/json/json.hpp` file to `ext/json.hpp`

### SPDLOG
1. Delete old ext/spdlog directory
2. Obtain package from https://github.com/gabime/spdlog/releases
2. Copy the include/spdlog directory into ext/spdlog
3. Copy the bundled fmt directory to /ext/fmt -- no need?

### FMT
1. Delete old ext/fmt directory
2. Get the latest release from https://github.com/fmtlib/fmt/releases
3. extract include/fmt into ext/fmt
