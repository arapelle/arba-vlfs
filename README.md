# Concept

The purpose is to provide C++ tool to manipulate virtual filesystem paths.

A virtual path is a std::filesystem::path matching the following pattern: `<VROOT><VIRTUAL_MARK><SUBPATH>`.

- `<VROOT>` is a string (Minimum length: 2. Maximum length: 8).
- `<VIRTUAL_MARK>` is a string separating the *virtual root* and the *subpath*. Its presence in the path indicates if the path is considered as a virtual filesystem path, or not. (default: `:/`)
- `<SUBPATH>` is a string describing a real filesystem path.

With `:/` as *virtual mark*, the path `rsc:/dir/subdir/file.txt` is a *virtual path* with `rsc` as root and `dir/subdir/file.txt` as subpath.

See [task board](https://app.gitkraken.com/glo/board/X2n1bz2bBQARwMBq) for future updates and features.

# Install

## Requirements

Binaries:

- A C++20 compiler (ex: g++-10)
- CMake 3.16 or later

Libraries:

- [strn](https://github.com/arapelle/strn) 0.1.4

- [Google Test](https://github.com/google/googletest) 1.10 or later (only for testing)

## Clone

```
git clone https://github.com/arapelle/vlfs --recurse-submodules
```

## Quick Install

There is a cmake script at the root of the project which builds the library in *Release* mode and install it (default options are used).

```
cd /path/to/vlfs
cmake -P cmake_quick_install.cmake
```

Use the following to quickly install a different mode.

```
cmake -DCMAKE_BUILD_TYPE=Debug -P cmake_quick_install.cmake
```

## Uninstall

There is a uninstall cmake script created during installation. You can use it to uninstall properly this library.

```
cd /path/to/installed-vlfs/
cmake -P cmake_uninstall.cmake
```

# How to use

## Example - Convert a virtual path to a real path

```c++
#include <iostream>
#include <arba/vlfs/vlfs.hpp>

using namespace strn::literals;

int main()
{
    vlfs::virtual_filesystem vfs;
    vfs.set_virtual_root("VROOT"_s64,  "/tmp");
    vfs.set_virtual_root("RSC"_s64,    "VROOT:/rsc");
    vfs.set_virtual_root("IMAGES"_s64, "RSC:/images");

    std::filesystem::path path("IMAGES:/file");
    vfs.convert_to_real_path(path);
    std::cout << path << std::endl;

    return EXIT_SUCCESS;
}
```

## Example - Using *vlfs* in a CMake project

See the [basic cmake project](https://github.com/arapelle/vlfs/tree/master/example/basic_cmake_project) example, and more specifically the [CMakeLists.txt](https://github.com/arapelle/vlfs/tree/master/example/basic_cmake_project/CMakeLists.txt) to see how to use *vlfs* in your CMake projects.

# License

[MIT License](https://github.com/arapelle/vlfs/blob/master/LICENSE.md) © vlfs