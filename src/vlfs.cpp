#include <arba/vlfs/vlfs.hpp>
#include <arba/strn/io.hpp>
#include <regex>
#include <stdexcept>
#include <iostream>

inline namespace arba
{
namespace vlfs
{

bool virtual_filesystem::is_virtual_root_name_valid(virtual_root_name vroot)
{
    static const std::regex virtual_root_regex(R"([a-zA-Z][\w.-]{1,7})",
                                               std::regex::ECMAScript|std::regex::optimize);
    return std::regex_match(vroot.begin(), vroot.end(), virtual_root_regex);
}

virtual_filesystem::virtual_filesystem()
{
    virtual_root_map_.emplace(temp_dir_vroot, std::filesystem::temp_directory_path());
    virtual_root_map_.emplace(old_temp_dir_vroot, std::filesystem::temp_directory_path());  // TO REMOVE
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
    virtual_root_map_.emplace(old_program_dir_vroot, root_path);  // TO REMOVE
    virtual_root_map_.emplace(program_dir_vroot, root_path);
    virtual_root_map_.emplace(canonical_program_dir_vroot, std::filesystem::canonical(root_path));
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
        res.virtual_root = strn_from_iter_len(path_begin, pos);
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
    if (path_components path_comps = extract_components(path); path_comps)
    {
        // if the root name is registered in the virtual filesystem,
        // we get it and convert it to real path:
        if (auto vroot_iter = virtual_root_map_.find(path_comps.virtual_root);
            vroot_iter != virtual_root_map_.end()) [[likely]]
        {
            if (path_comps.virtual_root == old_program_dir_vroot) [[unlikely]]  // THROW EXCEPTION // TO REMOVE
            {
                std::cerr << "WARNING: You are using a deprecated virtual root name: " << path_comps.virtual_root
                          << ". You should use \"" << program_dir_vroot << "\" instead." << std::endl;
            }
            else if (path_comps.virtual_root == old_temp_dir_vroot) [[unlikely]]  // THROW EXCEPTION // TO REMOVE
            {
                std::cerr << "WARNING: You are using a deprecated virtual root name: " << path_comps.virtual_root
                          << ". You should use \"" << temp_dir_vroot << "\" instead." << std::endl;
            }

            std::filesystem::path path = vroot_iter->second;
            convert_to_real_path(path);
            path /= path_comps.subpath;
            real_path = std::move(path);
        }
        else if (path_comps.virtual_root == current_dir_vroot) [[likely]]
        {
            real_path = std::filesystem::current_path() / path_comps.subpath;
        }
    }
}

std::filesystem::path virtual_filesystem::real_path(const path_components& path_comps)
{
    if (auto vroot_iter = virtual_root_map_.find(path_comps.virtual_root);
        vroot_iter != virtual_root_map_.end()) [[likely]]
    {
        std::filesystem::path vroot_path = vroot_iter->second;
        convert_to_real_path(vroot_path);
        return vroot_path / path_comps.subpath;
    }
    else if (path_comps.virtual_root == current_dir_vroot) [[likely]]
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
