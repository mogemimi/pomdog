// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include <Pomdog/Content/Utility/PathHelper.hpp>
#include <gtest/iutest_switch.hpp>

using namespace Pomdog::Detail;
using Pomdog::Detail::PathHelper;

TEST(PathHelper, Join)
{
    EXPECT_EQ("", PathHelper::Join("", ""));
    EXPECT_EQ("Chirico/Cuvie", PathHelper::Join("Chirico", "Cuvie"));
    EXPECT_EQ("Cuvie", PathHelper::Join("", "Cuvie"));
    EXPECT_EQ("Chirico", PathHelper::Join("Chirico", ""));
    EXPECT_EQ("PS/Chirico/Cuvie", PathHelper::Join("PS/Chirico", "Cuvie"));
    EXPECT_EQ("Chirico/Cuvie/AT", PathHelper::Join("Chirico", "Cuvie/AT"));
    EXPECT_EQ("PS/Chirico/Cuvie/AT", PathHelper::Join("PS/Chirico", "Cuvie/AT"));
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
