#include <arba/vlfs/version.hpp>
#include <gtest/gtest.h>

TEST(project_version_tests, test_version_core)
{
    constexpr unsigned major = 0;
    constexpr unsigned minor = 5;
    constexpr unsigned patch = 0;
    static_assert(arba::vlfs::version.core() == arba::cppx::numver(major, minor, patch));
}
