#include <arba/vlfs/vlfs.hpp> 

namespace vlfs
{
virtual_filesystem::virtual_filesystem(std::filesystem::path::string_type vroot_mark)
    : vroot_mark_(std::move(vroot_mark))
{}

void virtual_filesystem::set_virtual_root(const strn::string64& root_name, const std::filesystem::path& root_path)
{
    virtual_roots_[root_name] = root_path;
}

bool virtual_filesystem::is_virtual_path(const std::filesystem::path& path)
{
    std::size_t pos = std::string::npos;
    return is_virtual_path_(path, pos);
}

bool virtual_filesystem::is_virtual_path_ (const std::filesystem::path& path, std::size_t& pos)
{
    if (has_virtual_roots())
    {
        std::size_t path_view_length = std::min<std::size_t>(path.native().length(),
                                                             strn::string64::max_length() + vroot_mark_.length());
        path_string_view path_view(path.native().c_str(), path_view_length);
        pos = path_view.find(vroot_mark_);
        return pos != std::string::npos && pos >= 2;
    }
    return false;
}

std::filesystem::path virtual_filesystem::real_path (std::filesystem::path path)
{
    convert_to_real_path(path);
    return path;
}

void virtual_filesystem::convert_to_real_path (std::filesystem::path& path)
{
    std::size_t pos = std::string::npos;
    if (is_virtual_path_(path, pos))
    {
        // get root name:
        strn::string64 root_name;
        auto root_it = path.native().begin();
        auto root_end_it = root_it + static_cast<std::make_signed_t<std::size_t>>(pos);
        if constexpr(std::is_same_v<path_string_view, std::string_view>)
        {
            root_name = strn::string64(std::string_view(&*root_it, pos));
        }
        else
        {
            root_name = strn::string64(std::string(root_it, root_end_it));
        }

        // if the root name is registered int he virtual filesystem:
        auto vroot_iter = virtual_roots_.find(root_name);
        if (vroot_iter == virtual_roots_.end()) [[unlikely]]
            return;
        // we get it and convert it to real path:
        std::filesystem::path vroot_path = vroot_iter->second;
        convert_to_real_path(vroot_path);

        // we build the real path (realpath(root_name) + rest of path):
        auto subpath_begin_it = root_end_it + vroot_mark_.length();
        auto subpath_end_it = path.native().end();
        std::size_t subpath_length = static_cast<std::size_t>(std::distance(subpath_begin_it, subpath_end_it));
        path_string_view subpath(&*subpath_begin_it, subpath_length);
        path = vroot_path / subpath;
    }
}
}
