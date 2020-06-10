#include "Nucleus.hpp"
#include "MyApp.hpp"

int
main(int argc, char *argv[]) {

    std::cout << std::boolalpha;
    std::cout << "AppBase Copy: " << std::is_trivially_copyable<AppBase>::value << std::endl;
    std::cout << "MyApp Copy: " << std::is_trivially_copyable<MyApp>::value << std::endl;

    std::cout << "AppBase Destruct: " << std::is_trivially_destructible<AppBase>::value << std::endl;
    std::cout << "MyApp Destruct: " << std::is_trivially_destructible<MyApp>::value << std::endl;

    return nucleus::Nucleus<MyApp>(argc, argv).Run();

};
