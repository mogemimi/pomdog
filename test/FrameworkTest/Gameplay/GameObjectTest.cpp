//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <gtest/iutest_switch.hpp>
#include <cstdint>
#include <Pomdog/Gameplay/GameObject.hpp>

using Pomdog::GameObject;
using Pomdog::GameObjectID;

struct TransformInteger
{
	TransformInteger() = default;
	TransformInteger(int xIn, int yIn, int zIn)
		: x(xIn), y(yIn), z(zIn)
	{}

	int x, y, z;
};

TEST(GameObject, AddComponentWithoutArguments)
{
	GameObject gameObject;
	gameObject.AddComponent<int>();
	gameObject.AddComponent<std::uint16_t>();
	gameObject.AddComponent<std::uint32_t>();
	gameObject.AddComponent<std::uint64_t>();
	gameObject.AddComponent<std::string>();
	gameObject.AddComponent<TransformInteger>();
	
	EXPECT_TRUE(gameObject.HasComponent<int>());
	EXPECT_TRUE(gameObject.HasComponent<std::uint16_t>());
	EXPECT_TRUE(gameObject.HasComponent<std::uint32_t>());
	EXPECT_TRUE(gameObject.HasComponent<std::uint64_t>());
	EXPECT_TRUE(gameObject.HasComponent<std::string>());
	EXPECT_TRUE(gameObject.HasComponent<TransformInteger>());
	
	EXPECT_NE(nullptr, gameObject.Component<int>());
	EXPECT_NE(nullptr, gameObject.Component<std::uint16_t>());
	EXPECT_NE(nullptr, gameObject.Component<std::uint32_t>());
	EXPECT_NE(nullptr, gameObject.Component<std::uint64_t>());
	EXPECT_NE(nullptr, gameObject.Component<std::string>());
	EXPECT_NE(nullptr, gameObject.Component<TransformInteger>());
}

TEST(GameObject, AddComponentWithArguments)
{
	GameObject gameObject;
	gameObject.AddComponent<int>(42);
	gameObject.AddComponent<std::uint16_t>(42);
	gameObject.AddComponent<std::uint32_t>(42);
	gameObject.AddComponent<std::uint64_t>(42);
	gameObject.AddComponent<std::string>("Test");
	gameObject.AddComponent<TransformInteger>(3, 4, 5);
	
	ASSERT_NE(nullptr, gameObject.Component<int>());
	ASSERT_NE(nullptr, gameObject.Component<std::uint16_t>());
	ASSERT_NE(nullptr, gameObject.Component<std::uint32_t>());
	ASSERT_NE(nullptr, gameObject.Component<std::uint64_t>());
	ASSERT_NE(nullptr, gameObject.Component<std::string>());
	ASSERT_NE(nullptr, gameObject.Component<TransformInteger>());
	
	EXPECT_EQ(42, *gameObject.Component<int>());
	EXPECT_EQ(42, *gameObject.Component<std::uint16_t>());
	EXPECT_EQ(42, *gameObject.Component<std::uint32_t>());
	EXPECT_EQ(42, *gameObject.Component<std::uint64_t>());
	EXPECT_EQ("Test", *gameObject.Component<std::string>());
	EXPECT_EQ(3, gameObject.Component<TransformInteger>()->x);
	EXPECT_EQ(4, gameObject.Component<TransformInteger>()->y);
	EXPECT_EQ(5, gameObject.Component<TransformInteger>()->z);
}

TEST(GameObject, RemoveComponent)
{
	GameObject gameObject;
	gameObject.AddComponent<TransformInteger>();
	ASSERT_TRUE(gameObject.HasComponent<TransformInteger>());
	
	gameObject.RemoveComponent<TransformInteger>();
	EXPECT_FALSE(gameObject.HasComponent<TransformInteger>());
	EXPECT_EQ(nullptr, gameObject.Component<TransformInteger>());
}

TEST(GameObject, GameObjectID)
{
	GameObjectID const gameObjectID { 42 };
	GameObject gameObject { gameObjectID };
	
	EXPECT_EQ(gameObjectID, gameObject.ID());
	EXPECT_EQ(42, gameObject.ID().value);
}
