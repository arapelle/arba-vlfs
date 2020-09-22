#include <vlfs/vlfs.hpp>
#include <gtest/gtest.h>
#include <cstdlib>

using namespace strn::literals;

TEST(vlfs_tests, test_constructor_empty)
{
    vlfs::virtual_filesystem vlfs;
    ASSERT_FALSE(vlfs.has_virtual_roots());
    ASSERT_EQ(vlfs.virtual_roots().size(), 0);
    ASSERT_EQ(vlfs.virtual_root_mark(), ":/");
}

TEST(vlfs_tests, test_constructor)
{
    vlfs::virtual_filesystem vlfs("::");
    ASSERT_FALSE(vlfs.has_virtual_roots());
    ASSERT_EQ(vlfs.virtual_roots().size(), 0);
    ASSERT_EQ(vlfs.virtual_root_mark(), "::");
}

TEST(vlfs_tests, test_set_virtual_root)
{
    vlfs::virtual_filesystem vlfs;
    ASSERT_FALSE(vlfs.has_virtual_roots());
    ASSERT_EQ(vlfs.virtual_roots().size(), 0);
}

TEST(vlfs_tests, convert_to_real_path)
{
    vlfs::virtual_filesystem vfs;
    vfs.set_virtual_root("VROOT"_s64,  "/tmp");
    vfs.set_virtual_root("RSC"_s64,    "VROOT:/rsc");
    vfs.set_virtual_root("IMAGES"_s64, "RSC:/images");
    ASSERT_EQ(vfs.virtual_roots().size(), 3);

    std::filesystem::path path("IMAGES:/file");
    vfs.convert_to_real_path(path);
    ASSERT_EQ(path, "/tmp/rsc/images/file");

    path = std::filesystem::path ("VIDEOS:/file");
    vfs.convert_to_real_path(path);
    ASSERT_EQ(path, "VIDEOS:/file");
}

TEST(vlfs_tests, convert_to_real_path_with_custom_mark)
{
    vlfs::virtual_filesystem vfs(":://");
    vfs.set_virtual_root("VROOT"_s64,  "/tmp");
    vfs.set_virtual_root("RSC"_s64,    "VROOT:://rsc");
    vfs.set_virtual_root("IMAGES"_s64, "RSC:://images");

    std::filesystem::path path("IMAGES:://file");
    vfs.convert_to_real_path(path);
    ASSERT_EQ(path, "/tmp/rsc/images/file");
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
