#pragma once

#include <arba/strn/string64.hpp>

#include <filesystem>
#include <string_view>

inline namespace arba
{
namespace vlfs
{

class virtual_filesystem
{
public:
    using path_string = std::filesystem::path::string_type;
    using path_string_view = std::basic_string_view<path_string::value_type>;
    using virtual_root_name = strn::string64;
    using virtual_root_map = std::unordered_map<virtual_root_name, std::filesystem::path>;

public:
#ifdef WIN32
    static constexpr path_string_view virtual_root_marker = L":/";
    static_assert(std::is_same_v<std::filesystem::path::value_type, wchar_t>);
#else
    static constexpr path_string_view virtual_root_marker = ":/";
    static_assert(std::is_same_v<std::filesystem::path::value_type, char>);
#endif
    static constexpr virtual_root_name program_dir_vroot = "$PGMDIR";
    static constexpr virtual_root_name canonical_program_dir_vroot = "$CPGMDIR";
    static constexpr virtual_root_name temp_dir_vroot = "$TMPDIR";
    static constexpr virtual_root_name current_dir_vroot = "$CURDIR";

    // clang-format off
    [[deprecated("You should use program_dir_vroot (\"$PGMDIR\")!")]]
    static constexpr virtual_root_name old_program_dir_vroot = "$PROGDIR";
    [[deprecated("You should use temp_dir_vroot (\"$TMPDIR\")!")]]
    static constexpr virtual_root_name old_temp_dir_vroot = "$TMP";
    // clang-format on

public:
    virtual_filesystem();

    inline const virtual_root_map& virtual_map() const { return virtual_root_map_; }
    bool has_virtual_root(virtual_root_name vroot) const;
    void set_virtual_root(virtual_root_name vroot, const std::filesystem::path& root_path);
    void set_program_dir_virtual_root(const std::filesystem::path& program_dir_path);

    static bool is_virtual_path(path_string_view path);
    inline static bool is_virtual_path(const std::filesystem::path& path);

    struct path_components
    {
        virtual_root_name virtual_root;
        path_string_view subpath;

        inline operator bool() const noexcept { return virtual_root.not_empty(); }
    };
    path_components extract_components(path_string_view path);
    inline path_components extract_components(const std::filesystem::path& path);

    void convert_to_real_path(std::filesystem::path& path);
    inline std::filesystem::path real_path(path_string_view path);
    inline std::filesystem::path real_path(const std::filesystem::path& path);
    inline std::filesystem::path real_path(std::filesystem::path&& path);
    std::filesystem::path real_path(const path_components& path_comps);

    /**
     * @brief is_virtual_root_name_valid
     * @param vroot
     * @return true if vroot is a valid name, false else
     *
     * A valid name matches the following regular expression: [a-zA-Z][\w.-]{1,7}
     * example: RSC_VR, rsc.vr, rsc-VR
     */
    static bool is_virtual_root_name_valid(virtual_root_name vroot);

private:
    static bool is_virtual_path_(path_string_view path, std::size_t& pos);

private:
    virtual_root_map virtual_root_map_;
};

inline bool virtual_filesystem::is_virtual_path(const std::filesystem::path& path)
{
    return is_virtual_path(path_string_view(path.native()));
}

inline virtual_filesystem::path_components virtual_filesystem::extract_components(const std::filesystem::path& path)
{
    return extract_components(path_string_view(path.native()));
}

inline std::filesystem::path virtual_filesystem::real_path(path_string_view path)
{
    path_components path_comps = extract_components(path);
    if (path_comps)
        return real_path(path_comps);
    return path;
}

inline std::filesystem::path virtual_filesystem::real_path(const std::filesystem::path& path)
{
    return real_path(path_string_view(path.native()));
}

inline std::filesystem::path virtual_filesystem::real_path(std::filesystem::path&& path)
{
    std::filesystem::path real_path(std::move(path));
    convert_to_real_path(real_path);
    return real_path;
}

} // namespace vlfs
} // namespace arba
