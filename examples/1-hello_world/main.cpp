#include "Nucleus.hpp"
#include "MyApp.hpp"

int
main(int argc, char *argv[]) {

    std::cout << std::boolalpha;

    std::cout << "AppBase SLayout: " << std::is_standard_layout<AppBase>::value << std::endl;
    std::cout << "AppBase DConstruct: " << std::is_default_constructible<AppBase>::value << std::endl;
    std::cout << "AppBase TCopy: " << std::is_trivially_copyable<AppBase>::value << std::endl;
    std::cout << "AppBase TDestruct: " << std::is_trivially_destructible<AppBase>::value << std::endl << std::endl;

    std::cout << "MyApp SLayout: " << std::is_standard_layout<MyApp>::value << std::endl;
    std::cout << "MyApp DConstruct: " << std::is_default_constructible<MyApp>::value << std::endl;
    std::cout << "MyApp TCopy: " << std::is_trivially_copyable<MyApp>::value << std::endl;
    std::cout << "MyApp TDestruct: " << std::is_trivially_destructible<MyApp>::value << std::endl << std::endl;

    std::cout << "pmem::obj::string SLayout: " << std::is_standard_layout<pmem::obj::string>::value << std::endl;
    std::cout << "pmem::obj::string DConstruct: " << std::is_default_constructible<pmem::obj::string>::value << std::endl;
    std::cout << "pmem::obj::string TCopy: " << std::is_trivially_copyable<pmem::obj::string>::value << std::endl;
    std::cout << "pmem::obj::string TDestruct: " << std::is_trivially_destructible<pmem::obj::string>::value << std::endl << std::endl;

    return nucleus::Nucleus<MyApp>(argc, argv).Run();

};
