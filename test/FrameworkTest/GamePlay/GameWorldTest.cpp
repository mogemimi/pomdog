//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <gtest/iutest_switch.hpp>
#include <cstdint>
#include <Pomdog/GamePlay/GameWorld.hpp>
#include <Pomdog/GamePlay/GameObject.hpp>

using namespace Pomdog;

namespace {
	
struct Transform
{
	int x, y;
};

struct Collider
{
	int radius;
};

struct Texture3D;
	
}// unnamed namespace

TEST(GameWorld, AddChild)
{
	GameWorld world;

	auto gameObject = world.CreateObject();
	gameObject->AddComponent<Transform>(Transform{0, 0});
	gameObject->AddComponent<Collider>();
	
	EXPECT_FALSE(gameObject->HasComponent<Texture3D>());
	
	{
		auto gameObject2 = world.CreateObject();
		gameObject2->AddComponent<Transform>();
	}
	{
		auto gameObject3 = world.CreateObject();
		gameObject3->AddComponent<Collider>();
	}
	{
		auto gameObject4 = world.CreateObject();
	}
	
	auto objects = world.QueryComponents<Transform, Collider>();
	EXPECT_EQ(1, objects.size());

	for (auto gameObject: objects)
	{
		auto transform = gameObject->Component<Transform>();
		auto collider = gameObject->Component<Collider>();
		
		ASSERT_NE(nullptr, transform);
		transform->x = 480;
		transform->y = 320;
		
		ASSERT_NE(nullptr, collider);
		collider->radius = 50;
	}
	
	ASSERT_NE(nullptr, gameObject->Component<Transform>());
	EXPECT_EQ(480, gameObject->Component<Transform>()->x);
	EXPECT_EQ(320, gameObject->Component<Transform>()->y);
	
	ASSERT_NE(nullptr, gameObject->Component<Collider>());
	EXPECT_EQ(50, gameObject->Component<Collider>()->radius);
}


TEST(GameWorld, QueryComponents_Not)
{
	GameWorld world;
	{
		auto gameObject = world.CreateObject();
		gameObject->AddComponent<Transform>(Transform{0, 0});
		gameObject->AddComponent<Collider>();
	}
	{
		auto gameObject = world.CreateObject();
		gameObject->AddComponent<Transform>();
	}
	{
		auto gameObject = world.CreateObject();
		gameObject->AddComponent<Collider>();
	}
	{
		auto gameObject = world.CreateObject();
	}
	
	auto objects = world.QueryComponents<Transform, ComponentQuery::Not<Collider>>();
	EXPECT_EQ(1, objects.size());

	for (auto gameObject: objects)
	{
		EXPECT_NE(nullptr, gameObject->Component<Transform>());
		EXPECT_TRUE(gameObject->HasComponent<Transform>());
		
		EXPECT_EQ(nullptr, gameObject->Component<Collider>());
		EXPECT_FALSE(gameObject->HasComponent<Collider>());
	}
}
