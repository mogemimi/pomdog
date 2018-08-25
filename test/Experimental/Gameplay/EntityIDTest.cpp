// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog.Experimental/Gameplay/EntityID.hpp>
#include <gtest/iutest_switch.hpp>
#include <algorithm>
#include <cstdint>

using Pomdog::EntityID;

TEST(EntityID, NewEntityID)
{
    {
        EntityID id(1234, 5678);
        EXPECT_EQ(1234, id.SequenceNumber());
        EXPECT_EQ(5678, id.Index());
    }
    {
        EntityID id(0, 0);
        EXPECT_EQ(0, id.SequenceNumber());
        EXPECT_EQ(0, id.Index());
    }
    {
        EntityID id(std::numeric_limits<std::uint32_t>::max(),
            std::numeric_limits<std::uint32_t>::max());
        EXPECT_EQ(std::numeric_limits<std::uint32_t>::max(), id.SequenceNumber());
        EXPECT_EQ(std::numeric_limits<std::uint32_t>::max(), id.Index());
    }
    {
        EntityID id(std::numeric_limits<std::uint32_t>::max() - 1,
            std::numeric_limits<std::uint32_t>::max() - 2);
        EXPECT_EQ(std::numeric_limits<std::uint32_t>::max() - 1, id.SequenceNumber());
        EXPECT_EQ(std::numeric_limits<std::uint32_t>::max() - 2, id.Index());
    }
}
