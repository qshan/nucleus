#include "../../src/Config.hpp"
using namespace nucleus::config;

int main(int argc, char *argv[]) {
    auto config = Config("charlie");
    config.parse(argc, argv);

    return 0;

}
