// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Utility/PathHelper.hpp>
#include <Pomdog/Utility/FileSystem.hpp>
#include <Pomdog/Basic/Platform.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::PathHelper;
using Pomdog::FileSystem;

TEST(PathHelper, Join)
{
    EXPECT_EQ("", PathHelper::Join("", ""));
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    EXPECT_EQ("Chirico\\Cuvie", PathHelper::Join("Chirico", "Cuvie"));
    EXPECT_EQ("Cuvie", PathHelper::Join("", "Cuvie"));
    EXPECT_EQ("Chirico", PathHelper::Join("Chirico", ""));
    EXPECT_EQ("PS/Chirico\\Cuvie", PathHelper::Join("PS/Chirico", "Cuvie"));
    EXPECT_EQ("Chirico\\Cuvie/AT", PathHelper::Join("Chirico", "Cuvie/AT"));
    EXPECT_EQ("PS/Chirico\\Cuvie/AT", PathHelper::Join("PS/Chirico", "Cuvie/AT"));
#else
    EXPECT_EQ("Chirico/Cuvie", PathHelper::Join("Chirico", "Cuvie"));
    EXPECT_EQ("Cuvie", PathHelper::Join("", "Cuvie"));
    EXPECT_EQ("Chirico", PathHelper::Join("Chirico", ""));
    EXPECT_EQ("PS/Chirico/Cuvie", PathHelper::Join("PS/Chirico", "Cuvie"));
    EXPECT_EQ("Chirico/Cuvie/AT", PathHelper::Join("Chirico", "Cuvie/AT"));
    EXPECT_EQ("PS/Chirico/Cuvie/AT", PathHelper::Join("PS/Chirico", "Cuvie/AT"));
#endif
}

TEST(PathHelper, GetBaseName)
{
    EXPECT_EQ("Fiana", PathHelper::GetBaseName("Fiana"));
    EXPECT_EQ("Fiana", PathHelper::GetBaseName("Phantam Lady/Fiana"));
    EXPECT_EQ("Fiana", PathHelper::GetBaseName("Proto One/Phantam Lady/Fiana"));
    EXPECT_EQ("Fiana", PathHelper::GetBaseName("PS/Proto One/Phantam Lady/Fiana"));
}

TEST(PathHelper, GetDirectoryName)
{
    EXPECT_EQ("", PathHelper::GetDirectoryName("Fiana"));
    EXPECT_EQ("Phantam Lady", PathHelper::GetDirectoryName("Phantam Lady/Fiana"));
    EXPECT_EQ("Proto One/Phantam Lady", PathHelper::GetDirectoryName("Proto One/Phantam Lady/Fiana"));
    EXPECT_EQ("PS/Proto One/Phantam Lady", PathHelper::GetDirectoryName("PS/Proto One/Phantam Lady/Fiana"));
}

TEST(PathHelper, Split)
{
    auto result = PathHelper::Split("Fiana");
    EXPECT_EQ("", std::get<0>(result));
    EXPECT_EQ("Fiana", std::get<1>(result));

    result = PathHelper::Split("Phantam Lady/Fiana");
    EXPECT_EQ("Phantam Lady", std::get<0>(result));
    EXPECT_EQ("Fiana", std::get<1>(result));

    result = PathHelper::Split("Proto One/Phantam Lady/Fiana");
    EXPECT_EQ("Proto One/Phantam Lady", std::get<0>(result));
    EXPECT_EQ("Fiana", std::get<1>(result));

    result = PathHelper::Split("PS/Proto One/Phantam Lady/Fiana");
    EXPECT_EQ("PS/Proto One/Phantam Lady", std::get<0>(result));
    EXPECT_EQ("Fiana", std::get<1>(result));

    result = PathHelper::Split("PS/Proto One/Phantam Lady/Fiana.sotai");
    EXPECT_EQ("PS/Proto One/Phantam Lady", std::get<0>(result));
    EXPECT_EQ("Fiana.sotai", std::get<1>(result));
}

TEST(PathHelper, SplitExtension)
{
    auto result = PathHelper::SplitExtension("Fiana");
    EXPECT_EQ("Fiana", std::get<0>(result));
    EXPECT_EQ("", std::get<1>(result));

    result = PathHelper::SplitExtension("Phantam Lady.Fiana");
    EXPECT_EQ("Phantam Lady", std::get<0>(result));
    EXPECT_EQ("Fiana", std::get<1>(result));

    result = PathHelper::SplitExtension("Proto One/Phantam Lady.Fiana");
    EXPECT_EQ("Proto One/Phantam Lady", std::get<0>(result));
    EXPECT_EQ("Fiana", std::get<1>(result));

    result = PathHelper::SplitExtension("PS/Proto One/Phantam Lady.Fiana");
    EXPECT_EQ("PS/Proto One/Phantam Lady", std::get<0>(result));
    EXPECT_EQ("Fiana", std::get<1>(result));

    result = PathHelper::SplitExtension("PS/Proto One/Phantam Lady.Fiana.sotai");
    EXPECT_EQ("PS/Proto One/Phantam Lady.Fiana", std::get<0>(result));
    EXPECT_EQ("sotai", std::get<1>(result));

    result = PathHelper::SplitExtension("PS.Proto One/Phantam Lady.Fiana.sotai");
    EXPECT_EQ("PS.Proto One/Phantam Lady.Fiana", std::get<0>(result));
    EXPECT_EQ("sotai", std::get<1>(result));
}

TEST(PathHelper, Normalize)
{
    const auto normalize = PathHelper::Normalize;
    const auto cwd = FileSystem::GetCurrentWorkingDirectory();
    EXPECT_EQ(cwd, normalize(""));
    EXPECT_EQ(cwd, normalize("."));
    EXPECT_EQ(cwd, normalize("./"));

#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    EXPECT_EQ("/usr\\local\\bin", normalize("/usr/local/bin"));
    EXPECT_EQ("/usr\\local\\bin", normalize("/usr/local/bin/"));
    EXPECT_EQ("/usr\\local\\bin", normalize("/usr/local/bin/."));
    EXPECT_EQ("/usr\\local\\bin", normalize("/usr/local/bin/./"));
    EXPECT_EQ("/usr\\local\\bin", normalize("/usr/local/./bin"));
    EXPECT_EQ("/usr\\local\\bin", normalize("/usr/./local/bin"));
    EXPECT_EQ("/usr\\local", normalize("/usr/local/bin/.."));
    EXPECT_EQ("/usr\\local", normalize("/usr/local/bin/../"));
    EXPECT_EQ("/usr", normalize("/usr/local/bin/../.."));
    EXPECT_EQ("/usr", normalize("/usr/local/bin/../../"));
    EXPECT_EQ("/usr\\local\\bin", normalize("/usr/local/../local/bin"));
    EXPECT_EQ("/usr\\local", normalize("/usr/local/../local/bin/.."));
#else
    EXPECT_EQ("/usr/local/bin", normalize("/usr/local/bin"));
    EXPECT_EQ("/usr/local/bin", normalize("/usr/local/bin/"));
    EXPECT_EQ("/usr/local/bin", normalize("/usr/local/bin/."));
    EXPECT_EQ("/usr/local/bin", normalize("/usr/local/bin/./"));
    EXPECT_EQ("/usr/local/bin", normalize("/usr/local/./bin"));
    EXPECT_EQ("/usr/local/bin", normalize("/usr/./local/bin"));
    EXPECT_EQ("/usr/local", normalize("/usr/local/bin/.."));
    EXPECT_EQ("/usr/local", normalize("/usr/local/bin/../"));
    EXPECT_EQ("/usr", normalize("/usr/local/bin/../.."));
    EXPECT_EQ("/usr", normalize("/usr/local/bin/../../"));
    EXPECT_EQ("/usr/local/bin", normalize("/usr/local/../local/bin"));
    EXPECT_EQ("/usr/local", normalize("/usr/local/../local/bin/.."));
#endif
}

TEST(PathHelper, Relative)
{
    const auto relative = PathHelper::Relative;
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    EXPECT_EQ(".", relative("/usr/local/bin", "/usr/local/bin"));
    EXPECT_EQ(".", relative("/usr/local/bin/.", "/usr/local/bin"));
    EXPECT_EQ(".", relative("/usr/local/bin/./", "/usr/local/bin"));
    EXPECT_EQ(".", relative("/usr/local/bin/././", "/usr/local/bin"));
    EXPECT_EQ(".", relative("/usr/local/bin/./././", "/usr/local/bin"));
    EXPECT_EQ("..", relative("/usr/local/bin/..", "/usr/local/bin"));
    EXPECT_EQ("..", relative("/usr/local/bin/../", "/usr/local/bin"));
    EXPECT_EQ("..", relative("/usr/local/bin/.././", "/usr/local/bin"));
    EXPECT_EQ("..", relative("/usr/local/bin/./../", "/usr/local/bin"));
    EXPECT_EQ("..\\..", relative("/usr/local/bin/../../", "/usr/local/bin"));
    EXPECT_EQ("..\\..", relative("/usr/local/bin/../..", "/usr/local/bin"));
    EXPECT_EQ("..\\..", relative("/usr/local/bin/../../.", "/usr/local/bin"));
    EXPECT_EQ("..\\..", relative("/usr/local/bin/.././../", "/usr/local/bin"));
    EXPECT_EQ("..\\..", relative("/usr/local/bin/./../../", "/usr/local/bin"));

    EXPECT_EQ(".", relative("/usr/local/../local/bin", "/usr/local/bin"));
    EXPECT_EQ("..", relative("/usr/local", "/usr/local/bin"));
    EXPECT_EQ("..\\..", relative("/usr", "/usr/local/bin"));
    EXPECT_EQ("..\\..", relative("/usr/local/..", "/usr/local/bin"));
    EXPECT_EQ("..\\..", relative("/usr/local/../local/..", "/usr/local/bin"));

    EXPECT_EQ("gcc", relative("/usr/local/bin/gcc", "/usr/local/bin"));
    EXPECT_EQ("..\\opt", relative("/usr/local/opt", "/usr/local/bin"));
    EXPECT_EQ("..\\..\\share", relative("/usr/share", "/usr/local/bin"));
    EXPECT_EQ("..\\..\\share\\dict", relative("/usr/share/dict", "/usr/local/bin"));
#else
    EXPECT_EQ(".", relative("/usr/local/bin", "/usr/local/bin"));
    EXPECT_EQ(".", relative("/usr/local/bin/.", "/usr/local/bin"));
    EXPECT_EQ(".", relative("/usr/local/bin/./", "/usr/local/bin"));
    EXPECT_EQ(".", relative("/usr/local/bin/././", "/usr/local/bin"));
    EXPECT_EQ(".", relative("/usr/local/bin/./././", "/usr/local/bin"));
    EXPECT_EQ("..", relative("/usr/local/bin/..", "/usr/local/bin"));
    EXPECT_EQ("..", relative("/usr/local/bin/../", "/usr/local/bin"));
    EXPECT_EQ("..", relative("/usr/local/bin/.././", "/usr/local/bin"));
    EXPECT_EQ("..", relative("/usr/local/bin/./../", "/usr/local/bin"));
    EXPECT_EQ("../..", relative("/usr/local/bin/../../", "/usr/local/bin"));
    EXPECT_EQ("../..", relative("/usr/local/bin/../..", "/usr/local/bin"));
    EXPECT_EQ("../..", relative("/usr/local/bin/../../.", "/usr/local/bin"));
    EXPECT_EQ("../..", relative("/usr/local/bin/.././../", "/usr/local/bin"));
    EXPECT_EQ("../..", relative("/usr/local/bin/./../../", "/usr/local/bin"));

    EXPECT_EQ(".", relative("/usr/local/../local/bin", "/usr/local/bin"));
    EXPECT_EQ("..", relative("/usr/local", "/usr/local/bin"));
    EXPECT_EQ("../..", relative("/usr", "/usr/local/bin"));
    EXPECT_EQ("../..", relative("/usr/local/..", "/usr/local/bin"));
    EXPECT_EQ("../..", relative("/usr/local/../local/..", "/usr/local/bin"));

    EXPECT_EQ("gcc", relative("/usr/local/bin/gcc", "/usr/local/bin"));
    EXPECT_EQ("../opt", relative("/usr/local/opt", "/usr/local/bin"));
    EXPECT_EQ("../../share", relative("/usr/share", "/usr/local/bin"));
    EXPECT_EQ("../../share/dict", relative("/usr/share/dict", "/usr/local/bin"));
#endif
}
