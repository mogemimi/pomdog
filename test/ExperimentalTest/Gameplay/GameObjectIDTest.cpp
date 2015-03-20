// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include <Pomdog/Gameplay/GameObjectID.hpp>
#include <gtest/iutest_switch.hpp>
#include <cstdint>

using Pomdog::GameObjectID;

TEST(GameObjectID, NewGameObjectID)
{
	{
		GameObjectID id(1234, 5678);
		EXPECT_EQ(1234, id.SequenceNumber());
		EXPECT_EQ(5678, id.Index());
	}
	{
		GameObjectID id(0, 0);
		EXPECT_EQ(0, id.SequenceNumber());
		EXPECT_EQ(0, id.Index());
	}
	{
		GameObjectID id(std::numeric_limits<std::uint32_t>::max(),
			std::numeric_limits<std::uint32_t>::max());
		EXPECT_EQ(std::numeric_limits<std::uint32_t>::max(), id.SequenceNumber());
		EXPECT_EQ(std::numeric_limits<std::uint32_t>::max(), id.Index());
	}
	{
		GameObjectID id(std::numeric_limits<std::uint32_t>::max() - 1,
			std::numeric_limits<std::uint32_t>::max() - 2);
		EXPECT_EQ(std::numeric_limits<std::uint32_t>::max() - 1, id.SequenceNumber());
		EXPECT_EQ(std::numeric_limits<std::uint32_t>::max() - 2, id.Index());
	}
}
