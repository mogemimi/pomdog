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
#include <Pomdog/Utility/MakeUnique.hpp>

using Pomdog::Component;
using Pomdog::GameObject;
using Pomdog::GameObjectID;
using Pomdog::GameObjectContext;

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

TEST(GameObject, AddComponentWithoutArguments)
{
	auto objectContext = std::make_shared<GameObjectContext>();
	GameObject gameObject{objectContext};
	gameObject.AddComponent<TransformComponent>();
	gameObject.AddComponent<PhysicsComponent>();
	
	EXPECT_TRUE(gameObject.HasComponent<TransformComponent>());
	EXPECT_TRUE(gameObject.HasComponent<PhysicsComponent>());
	
	EXPECT_NE(nullptr, gameObject.Component<TransformComponent>());
	EXPECT_NE(nullptr, gameObject.Component<PhysicsComponent>());
}

TEST(GameObject, AddComponentWithArguments)
{
	auto objectContext = std::make_shared<GameObjectContext>();
	GameObject gameObject{objectContext};
	gameObject.AddComponent<TransformComponent>(3, 4, 5);
	gameObject.AddComponent<PhysicsComponent>(42);
	
	EXPECT_TRUE(gameObject.HasComponent<TransformComponent>());
	EXPECT_TRUE(gameObject.HasComponent<PhysicsComponent>());
	
	ASSERT_NE(nullptr, gameObject.Component<TransformComponent>());
	ASSERT_NE(nullptr, gameObject.Component<PhysicsComponent>());
	
	EXPECT_EQ(3, gameObject.Component<TransformComponent>()->x);
	EXPECT_EQ(4, gameObject.Component<TransformComponent>()->y);
	EXPECT_EQ(5, gameObject.Component<TransformComponent>()->z);
	EXPECT_EQ(42, gameObject.Component<PhysicsComponent>()->v);
}

TEST(GameObject, AddComponentWithInheritance)
{
	auto objectContext = std::make_shared<GameObjectContext>();
	GameObject gameObject{objectContext};
	MeshRendererComponent & meshRenderer = gameObject.AddComponent<MeshRendererComponent>(Pomdog::MakeUnique<MeshRendererComponent>());
	meshRenderer.SetZOrder(42);
	
	EXPECT_TRUE(gameObject.HasComponent<MeshRendererComponent>());
	ASSERT_NE(nullptr, gameObject.Component<MeshRendererComponent>());
	EXPECT_EQ(42, gameObject.Component<MeshRendererComponent>()->GetZOrder());
	
	EXPECT_TRUE(gameObject.HasComponent<RendererComponent>());
	ASSERT_NE(nullptr, gameObject.Component<RendererComponent>());
	auto renderer = gameObject.Component<RendererComponent>();
	renderer->SetZOrder(72);
	
	EXPECT_EQ(72, gameObject.Component<RendererComponent>()->GetZOrder());
	EXPECT_EQ(72, gameObject.Component<MeshRendererComponent>()->GetZOrder());
}

TEST(GameObject, RemoveComponent)
{
	auto objectContext = std::make_shared<GameObjectContext>();
	
	{
		GameObject gameObject{objectContext};
		gameObject.AddComponent<TransformComponent>();
		ASSERT_TRUE(gameObject.HasComponent<TransformComponent>());
		
		gameObject.RemoveComponent<TransformComponent>();
		EXPECT_FALSE(gameObject.HasComponent<TransformComponent>());
		EXPECT_EQ(nullptr, gameObject.Component<TransformComponent>());
	}
	{
		auto objectContext = std::make_shared<GameObjectContext>();
		GameObject gameObject{objectContext};
		gameObject.AddComponent<TransformComponent>(3, 4, 5);
		gameObject.AddComponent<PhysicsComponent>(42);
		
		EXPECT_TRUE(gameObject.HasComponent<TransformComponent>());
		EXPECT_TRUE(gameObject.HasComponent<PhysicsComponent>());
		
		ASSERT_NE(nullptr, gameObject.Component<TransformComponent>());
		ASSERT_NE(nullptr, gameObject.Component<PhysicsComponent>());
		
		gameObject.RemoveComponent<TransformComponent>();
		EXPECT_FALSE(gameObject.HasComponent<TransformComponent>());
		EXPECT_EQ(nullptr, gameObject.Component<TransformComponent>());
		
		gameObject.RemoveComponent<PhysicsComponent>();
		EXPECT_FALSE(gameObject.HasComponent<PhysicsComponent>());
		EXPECT_EQ(nullptr, gameObject.Component<PhysicsComponent>());
	}
}

TEST(GameObject, RemoveComponentWithInheritance)
{
	auto objectContext = std::make_shared<GameObjectContext>();
	{
		GameObject gameObject{objectContext};
		gameObject.AddComponent<MeshRendererComponent>(Pomdog::MakeUnique<MeshRendererComponent>());

		EXPECT_TRUE(gameObject.HasComponent<MeshRendererComponent>());
		EXPECT_NE(nullptr, gameObject.Component<MeshRendererComponent>());
		EXPECT_TRUE(gameObject.HasComponent<RendererComponent>());
		EXPECT_NE(nullptr, gameObject.Component<RendererComponent>());
		
		gameObject.RemoveComponent<MeshRendererComponent>();
		EXPECT_FALSE(gameObject.HasComponent<MeshRendererComponent>());
		EXPECT_EQ(nullptr, gameObject.Component<MeshRendererComponent>());
		EXPECT_FALSE(gameObject.HasComponent<RendererComponent>());
		EXPECT_EQ(nullptr, gameObject.Component<RendererComponent>());
	}
	{
		GameObject gameObject{objectContext};
		gameObject.AddComponent<MeshRendererComponent>(Pomdog::MakeUnique<MeshRendererComponent>());

		EXPECT_TRUE(gameObject.HasComponent<MeshRendererComponent>());
		EXPECT_NE(nullptr, gameObject.Component<MeshRendererComponent>());
		EXPECT_TRUE(gameObject.HasComponent<RendererComponent>());
		EXPECT_NE(nullptr, gameObject.Component<RendererComponent>());
		
		gameObject.RemoveComponent<RendererComponent>();
		EXPECT_FALSE(gameObject.HasComponent<MeshRendererComponent>());
		EXPECT_EQ(nullptr, gameObject.Component<MeshRendererComponent>());
		EXPECT_FALSE(gameObject.HasComponent<RendererComponent>());
		EXPECT_EQ(nullptr, gameObject.Component<RendererComponent>());
	}
}

TEST(GameObject, Component_Const)
{
	auto objectContext = std::make_shared<GameObjectContext>();
	auto gameObject = std::make_shared<GameObject>(objectContext);
	
	EXPECT_FALSE(gameObject->HasComponent<PhysicsComponent>());
	EXPECT_EQ(nullptr, gameObject->Component<PhysicsComponent>());
	
	gameObject->AddComponent<PhysicsComponent>(42);

	EXPECT_TRUE(gameObject->HasComponent<PhysicsComponent>());
	ASSERT_NE(nullptr, gameObject->Component<PhysicsComponent>());
	EXPECT_EQ(42, gameObject->Component<PhysicsComponent>()->v);
	
	{
		std::shared_ptr<GameObject const> gameObjectConstRef = gameObject;
		EXPECT_TRUE(gameObjectConstRef->HasComponent<PhysicsComponent>());
		ASSERT_NE(nullptr, gameObjectConstRef->Component<PhysicsComponent>());
		EXPECT_EQ(42, gameObjectConstRef->Component<PhysicsComponent>()->v);
	}
}

TEST(GameObject, GameObjectID)
{
	auto objectContext = std::make_shared<GameObjectContext>();
	GameObject gameObject {objectContext};
	
	auto id = gameObject.ID();
	EXPECT_EQ(id, gameObject.ID());
	
	GameObject gameObject2 {objectContext};
	
	EXPECT_NE(gameObject.ID(), gameObject2.ID());
	EXPECT_EQ(0, gameObject.ID().Index());
	EXPECT_EQ(1, gameObject2.ID().Index());
	EXPECT_EQ(1, gameObject.ID().SequenceNumber());
	EXPECT_EQ(1, gameObject2.ID().SequenceNumber());
}

TEST(GameObject, GameObjectID_Sequence)
{
	auto objectContext = std::make_shared<GameObjectContext>();
	auto gameObject = std::make_shared<GameObject>(objectContext);

	EXPECT_EQ(0, gameObject->ID().Index());
	EXPECT_NE(0, gameObject->ID().SequenceNumber());
	EXPECT_EQ(1, gameObject->ID().SequenceNumber());
	{
		auto gameObject = std::make_shared<GameObject>(objectContext);
		EXPECT_EQ(1, gameObject->ID().Index());
		EXPECT_NE(0, gameObject->ID().SequenceNumber());
		EXPECT_EQ(1, gameObject->ID().SequenceNumber());
	}
	{
		auto gameObject = std::make_shared<GameObject>(objectContext);
		EXPECT_EQ(1, gameObject->ID().Index());
		EXPECT_NE(0, gameObject->ID().SequenceNumber());
		EXPECT_EQ(2, gameObject->ID().SequenceNumber());
	}
	{
		auto gameObject = std::make_shared<GameObject>(objectContext);
		EXPECT_EQ(1, gameObject->ID().Index());
		EXPECT_NE(0, gameObject->ID().SequenceNumber());
		EXPECT_EQ(3, gameObject->ID().SequenceNumber());
	}
	{
		auto gameObject1 = std::make_shared<GameObject>(objectContext);
		EXPECT_EQ(1, gameObject1->ID().Index());
		EXPECT_NE(0, gameObject1->ID().SequenceNumber());
		EXPECT_EQ(4, gameObject1->ID().SequenceNumber());
		
		auto gameObject2 = std::make_shared<GameObject>(objectContext);
		EXPECT_EQ(2, gameObject2->ID().Index());
		EXPECT_NE(0, gameObject2->ID().SequenceNumber());
		EXPECT_EQ(1, gameObject2->ID().SequenceNumber());
		
		auto gameObject3 = std::make_shared<GameObject>(objectContext);
		EXPECT_EQ(3, gameObject3->ID().Index());
		EXPECT_NE(0, gameObject3->ID().SequenceNumber());
		EXPECT_EQ(1, gameObject3->ID().SequenceNumber());
	}
}

TEST(GameObject, GameObjectID_Unique)
{
	std::vector<std::shared_ptr<GameObject>> objects;
	std::vector<std::uint64_t> uniqueIdents;

	auto objectContext = std::make_shared<GameObjectContext>();

	std::mt19937 random;

	std::uint32_t maxSequenceNumber = 0;
	std::uint32_t maxIndex = 0;

	for (std::size_t count = 0; count < 262144; ++count)
	{
		if (count % 3 != 0)
		{
			auto gameObject = std::make_shared<GameObject>(objectContext);
			objects.push_back(gameObject);
			uniqueIdents.push_back(gameObject->ID().Value());
			
			//printf("### Create Object: %llu(%u, %u) \n",
			//	gameObject->ID().Value(),
			//	gameObject->ID().SequenceNumber(),
			//	gameObject->ID().Index());
			
			maxSequenceNumber = std::max(maxSequenceNumber, gameObject->ID().SequenceNumber());
			maxIndex = std::max(maxIndex, gameObject->ID().Index());
		}
		if (count % 11 == 8)
		{
			if (!objects.empty())
			{
				objects.erase(objects.begin());
				//printf("### Remove Object \n");
			}
		}
		if (count % 14 == 9)
		{
			if (!objects.empty())
			{
				std::uniform_int_distribution<std::uint32_t> distribution(1, 13);
				auto const randomNumber = distribution(random);
			
				objects.erase(std::remove_if(std::begin(objects), std::end(objects),
					[&](std::shared_ptr<Pomdog::GameObject> const& a) {
						return (a->ID().Value() % randomNumber == 0);
					}), std::end(objects));
				
				//printf("### Remove Objects \n");
			}
		}
	}
	
	//printf("## maxSequenceNumber = %u\n## maxIndex = %u\n", maxSequenceNumber, maxIndex);
	
	std::sort(std::begin(uniqueIdents), std::end(uniqueIdents));
	auto iter = std::adjacent_find(std::begin(uniqueIdents), std::end(uniqueIdents));
	
	EXPECT_EQ(std::end(uniqueIdents), iter);
}
