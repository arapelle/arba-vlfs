#pragma once

#include <arba/strn/string64.hpp>
#include <filesystem>
#include <string_view>
#include <string>

inline namespace arba
{
namespace vlfs
{

class virtual_filesystem
{
private:
    using path_string_view = std::basic_string_view<std::filesystem::path::value_type>;

public:
    using virtual_root_map = std::unordered_map<strn::string64, std::filesystem::path>;

public:
    explicit virtual_filesystem(std::filesystem::path::string_type vroot_mark_ = ":/");
    inline const virtual_root_map& virtual_roots() const { return virtual_roots_; }
    void set_virtual_root(const strn::string64& root_name, const std::filesystem::path& root_path);
    inline bool has_virtual_roots () const { return !virtual_roots_.empty(); }
    bool is_virtual_path(const std::filesystem::path& path);
    std::filesystem::path real_path(std::filesystem::path path);
    void convert_to_real_path(std::filesystem::path& path);

    inline const std::filesystem::path::string_type& virtual_root_mark() const { return vroot_mark_; }
    void set_virtual_root_mark(std::filesystem::path::string_type mark) { vroot_mark_ = std::move(mark); }

private:
    bool is_virtual_path_(const std::filesystem::path& path, std::size_t& pos);

private:
    virtual_root_map virtual_roots_;
    std::filesystem::path::string_type vroot_mark_;
};

}
}
