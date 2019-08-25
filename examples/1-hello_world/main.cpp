#include "Nucleus.hpp"
#include "MyApp.hpp"

int
main(int argc, char *argv[]) {

    return nucleus::Nucleus<MyApp>(argc, argv).Run();

};
