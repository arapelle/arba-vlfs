# Concept

The purpose is to provide C++ tools to manipulate virtual filesystem paths.

A virtual path is a std::filesystem::path matching the following pattern: `<VROOT><VIRTUAL_MARK><SUBPATH>`.

- `<VROOT>` is a string (Minimum length: 2. Maximum length: 8).
- `<VIRTUAL_MARK>` is a string separating the *virtual root* and the *subpath*. Its presence in the path indicates if the path is considered as a virtual filesystem path, or not. (default: `:/`)
- `<SUBPATH>` is a string describing a real filesystem path.

With `:/` as *virtual mark*, the path `rsc:/dir/subdir/file.txt` is a *virtual path* with `rsc` as root and `dir/subdir/file.txt` as subpath.

# Install

## Requirements

Binaries:
- A C++20 compiler (ex: g++-14)
- CMake 3.26 or later

Testing Libraries (optional):
- [Google Test](https://github.com/google/googletest) 1.14 or later  (optional)

## Clone

```
git clone https://github.com/arapelle/arba-vlfs
```

## Use with `conan`

Create the conan package.
```
conan create . --build=missing -c
```
Add a requirement in your conanfile project file.
```python
    def requirements(self):
        self.requires("arba-vlfs/0.5.0")
```

## Quick Install ##

There is a cmake script at the root of the project which builds the library in *Release* mode and install it (default options are used).

```
cd /path/to/arba-vlfs
cmake -P cmake/scripts/quick_install.cmake
```

Use the following to quickly install a different mode.

```
cmake -P cmake/scripts/quick_install.cmake -- TESTS BUILD Debug DIR /tmp/local
```

## Uninstall

There is a uninstall cmake script created during installation. You can use it to uninstall properly this library.

```
cd /path/to/installed-arba-vlfs/
cmake -P uninstall.cmake
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

# License

[MIT License](./LICENSE.md) © arba-vlfs
