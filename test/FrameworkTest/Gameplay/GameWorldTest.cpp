//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <gtest/iutest_switch.hpp>
#include <cstdint>
#include <Pomdog/Gameplay/GameWorld.hpp>
#include <Pomdog/Gameplay/GameObject.hpp>

using namespace Pomdog;

namespace {
	
struct TransformComponent: public Component<TransformComponent>
{
	TransformComponent() = default;
	TransformComponent(int xIn, int yIn, int zIn)
		: x(xIn), y(yIn), z(zIn)
	{}

	int x, y, z;
};

struct PhysicsComponent: public Component<PhysicsComponent>
{
	PhysicsComponent() = default;
	explicit PhysicsComponent(int vIn)
		: v(vIn)
	{}

	int v;
};

struct RendererComponent: public Component<RendererComponent>
{
	virtual ~RendererComponent() = default;
	
	virtual void SetZOrder(int z) = 0;
	virtual int GetZOrder() const = 0;
};

struct MeshRendererComponent final: public RendererComponent
{
	void SetZOrder(int zIn) override {
		this->z = zIn;
	}
	int GetZOrder() const override {
		return z;
	}

private:
	int z;
};
	
}// unnamed namespace

TEST(GameWorld, CreateObject)
{
	GameWorld world;
	{
		auto gameObject = world.CreateObject();
		gameObject.AddComponent<TransformComponent>();
		gameObject.AddComponent<PhysicsComponent>();
	}
	{
		auto gameObject = world.CreateObject();
		gameObject.AddComponent<TransformComponent>();
		gameObject.AddComponent<PhysicsComponent>();
	}
	{
		auto gameObject = world.CreateObject();
		gameObject.AddComponent<PhysicsComponent>();
	}
	{
		auto gameObject = world.CreateObject();
	}
	{
		auto gameObjects = world.QueryComponents<TransformComponent, PhysicsComponent>();
		EXPECT_EQ(2, gameObjects.size());
	}
	{
		auto gameObjects = world.QueryComponents<PhysicsComponent, TransformComponent>();
		EXPECT_EQ(2, gameObjects.size());
	}
	{
		auto gameObjects = world.QueryComponents<PhysicsComponent>();
		EXPECT_EQ(3, gameObjects.size());
	}
	{
		auto gameObjects = world.QueryComponents<TransformComponent>();
		EXPECT_EQ(2, gameObjects.size());
		
		for (auto & objects: gameObjects) {
			objects.DestroyImmediate();
		}
		
		gameObjects = world.QueryComponents<TransformComponent>();
		EXPECT_TRUE(gameObjects.empty());
	}
	{
		auto gameObjects = world.QueryComponents<TransformComponent, PhysicsComponent>();
		EXPECT_TRUE(gameObjects.empty());
	}
	{
		auto gameObjects = world.QueryComponents<PhysicsComponent, TransformComponent>();
		EXPECT_TRUE(gameObjects.empty());
	}
	{
		auto gameObjects = world.QueryComponents<PhysicsComponent>();
		EXPECT_EQ(1, gameObjects.size());
	}
}

//TEST(GameWorld, AddChild)
//{
//	GameWorld world;
//
//	auto gameObject = world.CreateObject();
//	gameObject.AddComponent<TransformComponent>();
//	gameObject.AddComponent<PhysicsComponent>();
//	
//	EXPECT_FALSE(gameObject->HasComponent<Texture3D>());
//	
//	{
//		auto gameObject2 = world.CreateObject();
//		gameObject2->AddComponent<Transform>();
//	}
//	{
//		auto gameObject3 = world.CreateObject();
//		gameObject3->AddComponent<Collider>();
//	}
//	{
//		auto gameObject4 = world.CreateObject();
//	}
//	
//	auto objects = world.QueryComponents<Transform, Collider>();
//	EXPECT_EQ(1, objects.size());
//
//	for (auto gameObject: objects)
//	{
//		auto transform = gameObject->Component<Transform>();
//		auto collider = gameObject->Component<Collider>();
//		
//		ASSERT_NE(nullptr, transform);
//		transform->x = 480;
//		transform->y = 320;
//		
//		ASSERT_NE(nullptr, collider);
//		collider->radius = 50;
//	}
//	
//	ASSERT_NE(nullptr, gameObject->Component<Transform>());
//	EXPECT_EQ(480, gameObject->Component<Transform>()->x);
//	EXPECT_EQ(320, gameObject->Component<Transform>()->y);
//	
//	ASSERT_NE(nullptr, gameObject->Component<Collider>());
//	EXPECT_EQ(50, gameObject->Component<Collider>()->radius);
//}
//
//
//TEST(GameWorld, QueryComponents_Not)
//{
//	GameWorld world;
//	{
//		auto gameObject = world.CreateObject();
//		gameObject->AddComponent<Transform>(Transform{0, 0});
//		gameObject->AddComponent<Collider>();
//	}
//	{
//		auto gameObject = world.CreateObject();
//		gameObject->AddComponent<Transform>();
//	}
//	{
//		auto gameObject = world.CreateObject();
//		gameObject->AddComponent<Collider>();
//	}
//	{
//		auto gameObject = world.CreateObject();
//	}
//	
//	auto objects = world.QueryComponents<Transform, ComponentQuery::Not<Collider>>();
//	EXPECT_EQ(1, objects.size());
//
//	for (auto gameObject: objects)
//	{
//		EXPECT_NE(nullptr, gameObject->Component<Transform>());
//		EXPECT_TRUE(gameObject->HasComponent<Transform>());
//		
//		EXPECT_EQ(nullptr, gameObject->Component<Collider>());
//		EXPECT_FALSE(gameObject->HasComponent<Collider>());
//	}
//}
