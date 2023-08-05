#include <arba/vlfs/vlfs.hpp>
#include <gtest/gtest.h>
#include <cstdlib>

using namespace std::string_view_literals;
using namespace strn::literals;

std::filesystem::path program_dir;

TEST(vlfs_tests, test_virtual_root_marker)
{
    ASSERT_EQ(vlfs::virtual_filesystem::virtual_root_marker[0], ':');
    ASSERT_EQ(vlfs::virtual_filesystem::virtual_root_marker[1], '/');
    ASSERT_EQ(vlfs::virtual_filesystem::virtual_root_marker.size(), 2);
}

TEST(vlfs_tests, test_constructor_empty)
{
    vlfs::virtual_filesystem vfs;
    ASSERT_EQ(vfs.virtual_map().size(), 1);
}

TEST(vlfs_tests, is_virtual_root_name_valid)
{
    ASSERT_FALSE(vlfs::virtual_filesystem::is_virtual_root_name_valid("$RSC")); // '$' is reserved for predefined roots
    ASSERT_FALSE(vlfs::virtual_filesystem::is_virtual_root_name_valid("R"));
    ASSERT_TRUE(vlfs::virtual_filesystem::is_virtual_root_name_valid("RA"));
    ASSERT_TRUE(vlfs::virtual_filesystem::is_virtual_root_name_valid("R_A"));
    ASSERT_TRUE(vlfs::virtual_filesystem::is_virtual_root_name_valid("R-A"));
    ASSERT_TRUE(vlfs::virtual_filesystem::is_virtual_root_name_valid("R.A"));
    ASSERT_FALSE(vlfs::virtual_filesystem::is_virtual_root_name_valid("_RA"));
    ASSERT_FALSE(vlfs::virtual_filesystem::is_virtual_root_name_valid("-RA"));
    ASSERT_FALSE(vlfs::virtual_filesystem::is_virtual_root_name_valid(".RA"));
}

TEST(vlfs_tests, is_virtual_path)
{
    vlfs::virtual_filesystem vfs;
    ASSERT_TRUE(vfs.is_virtual_path("RSC:/dir/file.txt"sv));
    ASSERT_FALSE(vfs.is_virtual_path("RSC:|dir/file.txt"sv)); // wrong virtual root marker
    ASSERT_FALSE(vfs.is_virtual_path("RESOURCES:/dir/file.txt"sv)); // too long virtual root name
}

TEST(vlfs_tests, convert_to_real_path)
{
    vlfs::virtual_filesystem vfs;
    vfs.set_virtual_root("VROOT"_s64,  "/tmp");
    vfs.set_virtual_root("RSC"_s64,    "VROOT:/rsc");
    vfs.set_virtual_root("IMAGES"_s64, "RSC:/images");
    ASSERT_EQ(vfs.virtual_map().size(), 4);
    ASSERT_TRUE(vfs.has_virtual_root("VROOT"_s64));
    ASSERT_FALSE(vfs.has_virtual_root("VOID"_s64));

    std::filesystem::path path("IMAGES:/file");
    vfs.convert_to_real_path(path);
    ASSERT_EQ(path, "/tmp/rsc/images/file");

    path = std::filesystem::path ("VIDEOS:/file");
    vfs.convert_to_real_path(path);
    ASSERT_EQ(path, "VIDEOS:/file");
}

TEST(vlfs_tests, convert_to_real_path_predefined_roots)
{
    vlfs::virtual_filesystem vfs;
    ASSERT_TRUE(vfs.has_virtual_root(vlfs::virtual_filesystem::current_dir_vroot));
    ASSERT_TRUE(vfs.has_virtual_root(vlfs::virtual_filesystem::temp_dir_vroot));
    ASSERT_FALSE(vfs.has_virtual_root(vlfs::virtual_filesystem::program_dir_vroot));

    ASSERT_EQ(vlfs::virtual_filesystem::current_dir_vroot, "$CURDIR"_s64);
    ASSERT_EQ(vlfs::virtual_filesystem::temp_dir_vroot, "$TMP"_s64);
    ASSERT_EQ(vlfs::virtual_filesystem::program_dir_vroot, "$PROGDIR"_s64);

    std::filesystem::path path("$CURDIR:/file");
    vfs.convert_to_real_path(path);
    ASSERT_STRNE(path.generic_string().c_str(), "$CURDIR:/file");

    path = std::filesystem::path("$TMP:/file");
    vfs.convert_to_real_path(path);
    ASSERT_STRNE(path.generic_string().c_str(), "$TMP:/file");

    path = std::filesystem::path("$PROGDIR:/file");
    vfs.convert_to_real_path(path);
    ASSERT_STREQ(path.generic_string().c_str(), "$PROGDIR:/file");

    vfs.set_program_dir_virtual_root(program_dir);
    ASSERT_TRUE(vfs.has_virtual_root(vlfs::virtual_filesystem::program_dir_vroot));
    vfs.convert_to_real_path(path);
    ASSERT_STRNE(path.generic_string().c_str(), "$PROGDIR:/file");
}

TEST(vlfs_tests, real_path)
{
    vlfs::virtual_filesystem vfs;
    std::filesystem::path v_path("file.txt");
    std::filesystem::path r_path = vfs.real_path(v_path);
    ASSERT_EQ(v_path, r_path);
}

TEST(vlfs_tests, extract_components)
{
    vlfs::virtual_filesystem vfs;
    vfs.set_virtual_root("RSC"_s64, "$CURDIR:/rsc");

    std::filesystem::path vpath("RSC:/dir/file.txt");
    ASSERT_TRUE(vfs.is_virtual_path(vpath));

    vlfs::virtual_filesystem::virtual_root_name vroot;
    vlfs::virtual_filesystem::path_string_view subpath;
    bool extract_res = vfs.extract_components(vpath, vroot, subpath);
    ASSERT_TRUE(extract_res);
    ASSERT_EQ(vroot, "RSC"_s64);
    ASSERT_EQ(std::filesystem::path(subpath), std::filesystem::path("dir/file.txt"));
}

int main(int argc, char **argv)
{
    program_dir = std::filesystem::canonical(*argv).parent_path();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
