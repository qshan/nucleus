# Supporting libraries

We are grateful for the contributions authors have made to providing these libraries as Open Source:

| Library | Source | Release* | Commit | Upd |
| ------- | ------- | ------ | -------| -------|
RESTINIO | https://github.com/Stiffstream/restinio | .0.6.0.1 |  74ebb558748dde009f57d2d9670ab970e9db9fb1 |*| 
HTTP_PARSER | https://github.com/nodejs/http-parser | 2.9.2 | 5c17dad400e45c5a442a63f250fff2638d144682 ||
ASIO | https://think-async.com/Asio/ | 1.12.2 | N/A ||
INIH | https://github.com/benhoyt/inih | r46>> | 0a87bf16693e35ed172ce8738404d9acd9545a5a |*| 
JSON.HPP | https://github.com/nlohmann/json | v3.7.0>> | f272ad533d32a40a3b2154a76f1ae9a45eacd6d3 |*|
SPDLOG | https://github.com/gabime/spdlog |  v1.4.2 | 1549ff12f1aa61ffc4d9a8727c519034724392a0 |*| 
FMT | https://github.com/fmtlib/fmt | 6.0.0 | Uses spdlog bundled version - see notes |*| 

\* In Release, \>\> indicates a later commit was taken than the release to resolve issues or use latest at sync

\* In Upd \>\> indicates a version update in current release

## Important
These libraries are subject to separate copyrights and are not included in the default copyright notice
at the root of the source tree. They are included here for your convenience. However, you are responsible for
ensuring ongoing compliance with the terms of these copyright owners.

## Update notes

### Restinio
1. Delete old ext/restinio directory
2. Copy the /dev/restinio directory into /ext
### HTTP_Parser

### ASIO
### INIH
1. Copy the contents of .c and .h into .cpp and .hpp files
2. Update #include in .cpp to reference inih.hpp

### JSON.HPP
1. Copy the `single_include/nlohhman/json/json.hpp` file to `ext/json.hpp`

### SPDLOG
1. Delete old ext/spdlog directory
2. Copy the include/spdlog directory into ext
3. Copy the bundled fmt directory to /ext/fmt

### FMT
1. Delete old ext/fmt directory
2. Copy the spdlog/fmt/bundled directory into ext
