#include <arba/vlfs/version.hpp>
#include <arba/vlfs/vlfs.hpp>

#include <iostream>

using namespace strn::literals;

int main()
{
    vlfs::virtual_filesystem vfs;
    vfs.set_virtual_root("VROOT"_s64, "/tmp");
    vfs.set_virtual_root("RSC"_s64, "VROOT:/rsc");
    vfs.set_virtual_root("IMAGES"_s64, "RSC:/images");

    std::filesystem::path path("IMAGES:/file");
    vfs.convert_to_real_path(path);
    std::cout << path << std::endl;

    std::cout << "TEST PACKAGE SUCCESS" << std::endl;
    return EXIT_SUCCESS;
}
