#include <arba/vlfs/vlfs.hpp>
#include <regex>
#include <stdexcept>

inline namespace arba
{
namespace vlfs
{

const virtual_filesystem::virtual_root_name virtual_filesystem::program_dir_vroot("$PROGDIR");
const virtual_filesystem::virtual_root_name virtual_filesystem::temp_dir_vroot("$TMP");
const virtual_filesystem::virtual_root_name virtual_filesystem::current_dir_vroot("$CURDIR");

bool virtual_filesystem::is_virtual_root_name_valid(virtual_root_name vroot)
{
    static const std::regex virtual_root_regex(R"([a-zA-Z][\w.-]{1,7})",
                                               std::regex::ECMAScript|std::regex::optimize);
    return std::regex_match(vroot.begin(), vroot.end(), virtual_root_regex);
}

virtual_filesystem::virtual_filesystem()
{
    virtual_root_map_.emplace(temp_dir_vroot, std::filesystem::temp_directory_path());
}

void virtual_filesystem::set_virtual_root(virtual_root_name vroot, const std::filesystem::path& root_path)
{
    if (is_virtual_root_name_valid(vroot)) [[likely]]
    {
        virtual_root_map_.insert_or_assign(vroot, root_path);
    }
    else
    {
        throw std::invalid_argument(vroot.to_string());
    }
}

void virtual_filesystem::set_program_dir_virtual_root(const std::filesystem::path& root_path)
{
    if (auto iter = virtual_root_map_.find(program_dir_vroot); iter != virtual_root_map_.end()) [[unlikely]]
        throw std::runtime_error("Program directory virtual root is already set.");
    virtual_root_map_.emplace(program_dir_vroot, root_path);
}

bool virtual_filesystem::has_virtual_root(virtual_root_name vroot) const
{
    if (auto vroot_iter = virtual_root_map_.find(vroot);
        vroot_iter != virtual_root_map_.end()) [[likely]]
        return true;
    if (vroot == current_dir_vroot) [[likely]]
        return true;
    return false;
}

bool virtual_filesystem::is_virtual_path(path_string_view path)
{
    std::size_t pos = std::string::npos;
    return is_virtual_path_(path, pos);
}

bool virtual_filesystem::is_virtual_path_(path_string_view path, std::size_t& pos)
{
    std::size_t path_view_length = std::min<std::size_t>(path.length(),
                                                         strn::string64::max_length()
                                                             + virtual_root_marker.length());
    path_string_view bounded_path_view(path.data(), path_view_length);
    pos = bounded_path_view.find(virtual_root_marker);
    return pos != std::string::npos && pos >= 2;
}

namespace
{
template <class T>
requires requires(T iter, std::size_t n)
{
    std::string_view(iter, n);
}
strn::string64 strn_from_iter_len(T iter, std::size_t n)
{
    return strn::string64(std::string_view(&*iter, n));
}

template <class T>
requires (!requires(T iter, std::size_t n)
{
    std::string_view(iter, n);
})
strn::string64 strn_from_iter_len(T iter, std::size_t n)
{
    return strn::string64(std::string(iter, iter + n));
}
}

virtual_filesystem::path_components virtual_filesystem::extract_components(path_string_view path_view)
{
    virtual_filesystem::path_components res;

    if (std::size_t pos; is_virtual_path_(path_view, pos))
    {
        auto path_begin = path_view.begin();
        res.vroot = strn_from_iter_len(path_begin, pos);
        auto subpath_begin_it = path_begin + pos + virtual_root_marker.length();
        std::size_t subpath_length = std::distance(subpath_begin_it, path_view.end());
        res.subpath = path_string_view(&*subpath_begin_it, subpath_length);
    }
    else
        res.subpath = path_view;

    return res;
}

void virtual_filesystem::convert_to_real_path(std::filesystem::path& real_path)
{
    path_string_view path(real_path.native());
    if (auto path_comps = extract_components(path); path_comps)
    {
        // if the root name is registered in the virtual filesystem,
        // we get it and convert it to real path:
        if (auto vroot_iter = virtual_root_map_.find(path_comps.vroot);
            vroot_iter != virtual_root_map_.end()) [[likely]]
        {
            std::filesystem::path vroot_path = vroot_iter->second;
            convert_to_real_path(vroot_path);
            real_path = vroot_path / path_comps.subpath;
        }
        else if (path_comps.vroot == current_dir_vroot) [[likely]]
        {
            real_path = std::filesystem::current_path() / path_comps.subpath;
        }
    }
}

std::filesystem::path virtual_filesystem::real_path(const path_components& path_comps)
{
    if (auto vroot_iter = virtual_root_map_.find(path_comps.vroot);
        vroot_iter != virtual_root_map_.end()) [[likely]]
    {
        std::filesystem::path vroot_path = vroot_iter->second;
        convert_to_real_path(vroot_path);
        return vroot_path / path_comps.subpath;
    }
    else if (path_comps.vroot == current_dir_vroot) [[likely]]
    {
        return std::filesystem::current_path() / path_comps.subpath;
    }
    else
    {
        return std::filesystem::path(path_comps.subpath);
    }
}

}
}
