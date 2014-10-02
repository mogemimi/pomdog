//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Gameplay/GameObject.hpp>
#include <gtest/iutest_switch.hpp>
#include <cstdint>
#include <memory>

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
	MeshRendererComponent & meshRenderer = gameObject.AddComponent<MeshRendererComponent>(std::make_unique<MeshRendererComponent>());
	meshRenderer.SetZOrder(42);
	
	//EXPECT_TRUE(gameObject.HasComponent<MeshRendererComponent>());
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
	{
		auto objectContext = std::make_shared<GameObjectContext>();
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
		gameObject.AddComponent<MeshRendererComponent>(std::make_unique<MeshRendererComponent>());

		//EXPECT_TRUE(gameObject.HasComponent<MeshRendererComponent>());
		EXPECT_NE(nullptr, gameObject.Component<MeshRendererComponent>());
		EXPECT_TRUE(gameObject.HasComponent<RendererComponent>());
		EXPECT_NE(nullptr, gameObject.Component<RendererComponent>());
		
		gameObject.RemoveComponent<MeshRendererComponent>();
		//EXPECT_FALSE(gameObject.HasComponent<MeshRendererComponent>());
		EXPECT_EQ(nullptr, gameObject.Component<MeshRendererComponent>());
		EXPECT_FALSE(gameObject.HasComponent<RendererComponent>());
		EXPECT_EQ(nullptr, gameObject.Component<RendererComponent>());
	}
	{
		GameObject gameObject{objectContext};
		gameObject.AddComponent<MeshRendererComponent>(std::make_unique<MeshRendererComponent>());

		//EXPECT_TRUE(gameObject.HasComponent<MeshRendererComponent>());
		EXPECT_NE(nullptr, gameObject.Component<MeshRendererComponent>());
		EXPECT_TRUE(gameObject.HasComponent<RendererComponent>());
		EXPECT_NE(nullptr, gameObject.Component<RendererComponent>());
		
		gameObject.RemoveComponent<RendererComponent>();
		//EXPECT_FALSE(gameObject.HasComponent<MeshRendererComponent>());
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

TEST(GameObject, DestroyImmediate)
{
	{
		auto objectContext = std::make_shared<GameObjectContext>();
		GameObject gameObject {objectContext};
		
		EXPECT_TRUE(gameObject);
		EXPECT_EQ(1, objectContext->Count());
		gameObject.DestroyImmediate();
		EXPECT_FALSE(gameObject);
		EXPECT_EQ(0, objectContext->Count());
	}
	{
		auto objectContext = std::make_shared<GameObjectContext>();
		GameObject gameObject {objectContext};
		
		EXPECT_TRUE(gameObject);
		EXPECT_EQ(1, objectContext->Count());
		objectContext->DestroyImmediate(gameObject.ID());
		EXPECT_FALSE(gameObject);
		EXPECT_EQ(0, objectContext->Count());
	}
	{
		auto objectContext = std::make_shared<GameObjectContext>();
		GameObject gameObject1 {objectContext};
		GameObject gameObject2 {objectContext};
		GameObject gameObject3 {objectContext};
			
		EXPECT_TRUE(gameObject1);
		EXPECT_TRUE(gameObject2);
		EXPECT_TRUE(gameObject3);
		EXPECT_EQ(3, objectContext->Count());
		gameObject1.DestroyImmediate();
		EXPECT_FALSE(gameObject1);
		EXPECT_TRUE(gameObject2);
		EXPECT_TRUE(gameObject3);
		EXPECT_EQ(2, objectContext->Count());
		gameObject3.DestroyImmediate();
		EXPECT_FALSE(gameObject1);
		EXPECT_TRUE(gameObject2);
		EXPECT_FALSE(gameObject3);
		EXPECT_EQ(1, objectContext->Count());
		gameObject2.DestroyImmediate();
		EXPECT_FALSE(gameObject1);
		EXPECT_FALSE(gameObject2);
		EXPECT_FALSE(gameObject3);
		EXPECT_EQ(0, objectContext->Count());
	}
	
	struct Behavior: public Pomdog::Component<Behavior> {
		void Do(Pomdog::GameObject & self)
		{
			EXPECT_TRUE(self);
			EXPECT_TRUE(self.HasComponent<Behavior>());
			self.DestroyImmediate();
			EXPECT_FALSE(self);
			//EXPECT_FALSE(self.HasComponent<Behavior>());
		}
		
		std::shared_ptr<int> ptr = std::make_shared<int>(42);
	};
	
	{
		auto objectContext = std::make_shared<GameObjectContext>();
		GameObject gameObject {objectContext};
		auto & behavior = gameObject.AddComponent<Behavior>();
		std::weak_ptr<int> weak = behavior.ptr;
		
		EXPECT_TRUE(gameObject);
		EXPECT_TRUE(gameObject.HasComponent<Behavior>());
		EXPECT_EQ(&behavior, gameObject.Component<Behavior>());
		EXPECT_FALSE(weak.expired());
		EXPECT_EQ(1, objectContext->Count());
		behavior.Do(gameObject);
		EXPECT_FALSE(gameObject);
		//EXPECT_FALSE(gameObject.HasComponent<Behavior>());
		//EXPECT_EQ(nullptr, gameObject.Component<Behavior>());
		EXPECT_TRUE(weak.expired());
		EXPECT_EQ(0, objectContext->Count());
	}
}

TEST(GameObject, Destroy)
{
	{
		auto objectContext = std::make_shared<GameObjectContext>();
		GameObject gameObject {objectContext};
		
		EXPECT_TRUE(gameObject);
		EXPECT_EQ(1, objectContext->Count());
		gameObject.Destroy();
		EXPECT_FALSE(gameObject);
		EXPECT_EQ(0, objectContext->Count());
		objectContext->Refresh();
		EXPECT_FALSE(gameObject);
		EXPECT_EQ(0, objectContext->Count());
	}
	{
		auto objectContext = std::make_shared<GameObjectContext>();
		GameObject gameObject {objectContext};
		
		EXPECT_TRUE(gameObject);
		EXPECT_EQ(1, objectContext->Count());
		objectContext->Destroy(gameObject.ID());
		EXPECT_FALSE(gameObject);
		EXPECT_EQ(0, objectContext->Count());
		objectContext->Refresh();
		EXPECT_FALSE(gameObject);
		EXPECT_EQ(0, objectContext->Count());
	}
	{
		auto objectContext = std::make_shared<GameObjectContext>();
		GameObject gameObject1 {objectContext};
		GameObject gameObject2 {objectContext};
		GameObject gameObject3 {objectContext};
			
		EXPECT_TRUE(gameObject1);
		EXPECT_TRUE(gameObject2);
		EXPECT_TRUE(gameObject3);
		EXPECT_EQ(3, objectContext->Count());
		gameObject1.Destroy();
		EXPECT_FALSE(gameObject1);
		EXPECT_TRUE(gameObject2);
		EXPECT_TRUE(gameObject3);
		EXPECT_EQ(2, objectContext->Count());
		gameObject3.Destroy();
		EXPECT_FALSE(gameObject1);
		EXPECT_TRUE(gameObject2);
		EXPECT_FALSE(gameObject3);
		EXPECT_EQ(1, objectContext->Count());
		gameObject2.Destroy();
		EXPECT_FALSE(gameObject1);
		EXPECT_FALSE(gameObject2);
		EXPECT_FALSE(gameObject3);
		EXPECT_EQ(0, objectContext->Count());
		objectContext->Refresh();
		EXPECT_FALSE(gameObject1);
		EXPECT_FALSE(gameObject2);
		EXPECT_FALSE(gameObject3);
		EXPECT_EQ(0, objectContext->Count());
	}
	
	struct Behavior: public Pomdog::Component<Behavior> {
		void Do(Pomdog::GameObject & self)
		{
			EXPECT_TRUE(self);
			EXPECT_TRUE(self.HasComponent<Behavior>());
			self.Destroy();
			EXPECT_FALSE(self);
			//EXPECT_FALSE(self.HasComponent<Behavior>());
		}
		
		std::shared_ptr<int> ptr = std::make_shared<int>(42);
	};
	
	{
		auto objectContext = std::make_shared<GameObjectContext>();
		GameObject gameObject {objectContext};
		auto & behavior = gameObject.AddComponent<Behavior>();
		std::weak_ptr<int> weak = behavior.ptr;
		
		EXPECT_TRUE(gameObject);
		EXPECT_TRUE(gameObject.HasComponent<Behavior>());
		EXPECT_EQ(&behavior, gameObject.Component<Behavior>());
		EXPECT_FALSE(weak.expired());
		EXPECT_EQ(1, objectContext->Count());
		behavior.Do(gameObject);
		EXPECT_FALSE(gameObject);
		//EXPECT_FALSE(gameObject.HasComponent<Behavior>());
		//EXPECT_EQ(nullptr, gameObject.Component<Behavior>());
		EXPECT_FALSE(weak.expired());
		EXPECT_EQ(0, objectContext->Count());
		objectContext->Refresh();
		EXPECT_FALSE(gameObject);
		//EXPECT_FALSE(gameObject.HasComponent<Behavior>());
		//EXPECT_EQ(nullptr, gameObject.Component<Behavior>());
		EXPECT_TRUE(weak.expired());
		EXPECT_EQ(0, objectContext->Count());
	}
}

TEST(GameObject, Clear)
{
	{
		auto objectContext = std::make_shared<GameObjectContext>();
		GameObject gameObject1 {objectContext};
		GameObject gameObject2 {objectContext};
		GameObject gameObject3 {objectContext};
		
		EXPECT_TRUE(gameObject1);
		EXPECT_TRUE(gameObject2);
		EXPECT_TRUE(gameObject3);
		EXPECT_EQ(3, objectContext->Count());
		objectContext->Clear();
		EXPECT_FALSE(gameObject1);
		EXPECT_FALSE(gameObject2);
		EXPECT_FALSE(gameObject3);
		EXPECT_EQ(0, objectContext->Count());
	}
	{
		auto objectContext = std::make_shared<GameObjectContext>();
		GameObject gameObject1 {objectContext};
		GameObject gameObject2 {objectContext};
		GameObject gameObject3 {objectContext};
		
		gameObject1.AddComponent<TransformComponent>();
		gameObject1.AddComponent<PhysicsComponent>();
		gameObject2.AddComponent<TransformComponent>();
		gameObject3.AddComponent<PhysicsComponent>();

		EXPECT_TRUE(gameObject1);
		EXPECT_TRUE(gameObject2);
		EXPECT_TRUE(gameObject3);
		EXPECT_NE(gameObject1.ID(), gameObject2.ID());
		EXPECT_NE(gameObject2.ID(), gameObject3.ID());
		EXPECT_EQ(3, objectContext->Count());
		objectContext->Clear();
		EXPECT_FALSE(gameObject1);
		EXPECT_FALSE(gameObject2);
		EXPECT_FALSE(gameObject3);
		EXPECT_EQ(0, objectContext->Count());
	}
	{
		auto objectContext = std::make_shared<GameObjectContext>();
		GameObject gameObject {objectContext};
		auto oldId = gameObject.ID();
		
		EXPECT_TRUE(gameObject);
		EXPECT_EQ(1, objectContext->Count());
		objectContext->Clear();
		EXPECT_FALSE(gameObject);
		EXPECT_EQ(0, objectContext->Count());
		
		gameObject = GameObject{objectContext};
		auto newId = gameObject.ID();
		EXPECT_NE(oldId, newId);
		EXPECT_NE(oldId.SequenceNumber(), newId.SequenceNumber());
		EXPECT_EQ(oldId.Index(), newId.Index());
	}
	
	struct Behavior: public Pomdog::Component<Behavior> {
		std::shared_ptr<int> ptr = std::make_shared<int>(42);
	};
	
	{
		auto objectContext = std::make_shared<GameObjectContext>();
		GameObject gameObject {objectContext};
		auto & behavior = gameObject.AddComponent<Behavior>();
		std::weak_ptr<int> weak = behavior.ptr;
		
		EXPECT_TRUE(gameObject);
		EXPECT_TRUE(gameObject.HasComponent<Behavior>());
		EXPECT_EQ(&behavior, gameObject.Component<Behavior>());
		EXPECT_FALSE(weak.expired());
		EXPECT_EQ(1, objectContext->Count());
		objectContext->Clear();
		EXPECT_FALSE(gameObject);
		EXPECT_TRUE(weak.expired());
		EXPECT_EQ(0, objectContext->Count());
	}
	{
		auto objectContext = std::make_shared<GameObjectContext>();
		GameObject gameObject1 {objectContext};
		GameObject gameObject2 {objectContext};
		auto & behavior = gameObject1.AddComponent<Behavior>();
		std::weak_ptr<int> weak = behavior.ptr;
		
		auto oldId1 = gameObject1.ID();
		auto oldId2 = gameObject2.ID();
		
		EXPECT_TRUE(gameObject1);
		EXPECT_TRUE(gameObject2);
		EXPECT_EQ(2, objectContext->Count());
		objectContext->Clear();
		EXPECT_FALSE(gameObject1);
		EXPECT_FALSE(gameObject2);
		EXPECT_EQ(0, objectContext->Count());
		
		gameObject1 = GameObject{objectContext};
		gameObject2 = GameObject{objectContext};
		
		EXPECT_NE(oldId1, oldId2);
		EXPECT_NE(oldId1, gameObject1.ID());
		EXPECT_NE(oldId2, gameObject2.ID());
		EXPECT_NE(oldId1.SequenceNumber(), gameObject1.ID().SequenceNumber());
		EXPECT_NE(oldId2.SequenceNumber(), gameObject2.ID().SequenceNumber());
		EXPECT_EQ(oldId1.Index(), gameObject1.ID().Index());
		EXPECT_EQ(oldId2.Index(), gameObject2.ID().Index());
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
	{
		GameObject gameObject {objectContext};
		EXPECT_EQ(0, gameObject.ID().Index());
		EXPECT_NE(0, gameObject.ID().SequenceNumber());
		EXPECT_EQ(1, gameObject.ID().SequenceNumber());
	}
	{
		GameObject gameObject {objectContext};
		EXPECT_EQ(1, gameObject.ID().Index());
		EXPECT_NE(0, gameObject.ID().SequenceNumber());
		EXPECT_EQ(1, gameObject.ID().SequenceNumber());
		gameObject.DestroyImmediate();
	}
	{
		GameObject gameObject {objectContext};
		EXPECT_EQ(1, gameObject.ID().Index());
		EXPECT_NE(0, gameObject.ID().SequenceNumber());
		EXPECT_EQ(2, gameObject.ID().SequenceNumber());
		gameObject.DestroyImmediate();
	}
	{
		GameObject gameObject {objectContext};
		EXPECT_EQ(1, gameObject.ID().Index());
		EXPECT_NE(0, gameObject.ID().SequenceNumber());
		EXPECT_EQ(3, gameObject.ID().SequenceNumber());
		gameObject.DestroyImmediate();
	}
	{
		GameObject gameObject1 {objectContext};
		EXPECT_EQ(1, gameObject1.ID().Index());
		EXPECT_NE(0, gameObject1.ID().SequenceNumber());
		EXPECT_EQ(4, gameObject1.ID().SequenceNumber());
		
		GameObject gameObject2 {objectContext};
		EXPECT_EQ(2, gameObject2.ID().Index());
		EXPECT_NE(0, gameObject2.ID().SequenceNumber());
		EXPECT_EQ(1, gameObject2.ID().SequenceNumber());
		
		GameObject gameObject3 {objectContext};
		EXPECT_EQ(3, gameObject3.ID().Index());
		EXPECT_NE(0, gameObject3.ID().SequenceNumber());
		EXPECT_EQ(1, gameObject3.ID().SequenceNumber());
		
		gameObject1.DestroyImmediate();
		gameObject2.DestroyImmediate();
		gameObject3.DestroyImmediate();
	}
}

TEST(GameObject, Cast_Bool)
{
	auto objectContext = std::make_shared<GameObjectContext>();
	{
		GameObject gameObject {objectContext};
		EXPECT_TRUE(gameObject);
		gameObject.DestroyImmediate();
		EXPECT_FALSE(gameObject);
	}
	{
		GameObject gameObject1 {objectContext};
		GameObject gameObject2 {objectContext};
		EXPECT_TRUE(gameObject1);
		EXPECT_TRUE(gameObject2);
		gameObject1.DestroyImmediate();
		EXPECT_FALSE(gameObject1);
		EXPECT_TRUE(gameObject2);
		gameObject2.DestroyImmediate();
		EXPECT_FALSE(gameObject1);
		EXPECT_FALSE(gameObject2);
	}
	{
		GameObject gameObject {objectContext};
		auto copiedObject = gameObject;
		EXPECT_TRUE(gameObject);
		EXPECT_TRUE(copiedObject);
		gameObject.DestroyImmediate();
		EXPECT_FALSE(gameObject);
		EXPECT_FALSE(copiedObject);
	}
	{
		GameObject gameObject {objectContext};
		auto copiedObject = gameObject;
		EXPECT_TRUE(gameObject);
		EXPECT_TRUE(copiedObject);
		copiedObject.DestroyImmediate();
		EXPECT_FALSE(gameObject);
		EXPECT_FALSE(copiedObject);
	}
}

TEST(GameObject, GameObjectID_Unique)
{
	std::vector<GameObject> objects;
	std::vector<std::uint64_t> uniqueIdents;

	auto objectContext = std::make_shared<GameObjectContext>();

	std::mt19937 random;

	std::uint32_t maxSequenceNumber = 0;
	std::uint32_t maxIndex = 0;

	for (std::size_t count = 0; count < 262144; ++count)
	{
		if (count % 3 != 0)
		{
			GameObject gameObject{objectContext};
			objects.push_back(gameObject);
			uniqueIdents.push_back(gameObject.ID().Value());
			
			//printf("### Create Object: %llu(%u, %u) \n",
			//	gameObject->ID().Value(),
			//	gameObject->ID().SequenceNumber(),
			//	gameObject->ID().Index());
			
			maxSequenceNumber = std::max(maxSequenceNumber, gameObject.ID().SequenceNumber());
			maxIndex = std::max(maxIndex, gameObject.ID().Index());
		}
		if (count % 11 == 8)
		{
			if (!objects.empty())
			{
				ASSERT_TRUE(objects.front());
				objects.front().DestroyImmediate();
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
			
				for (auto & object: objects)
				{
					ASSERT_TRUE(object);
					if (object.ID().Value() % randomNumber == 0) {
						object.DestroyImmediate();
					}
				}
				objects.erase(std::remove_if(std::begin(objects), std::end(objects),
					[&](GameObject const& object) { return !object; }), std::end(objects));
				
				//printf("### Remove Objects \n");
			}
		}
	}
	
	//printf("## maxSequenceNumber = %u\n## maxIndex = %u\n", maxSequenceNumber, maxIndex);
	
	ASSERT_FALSE(uniqueIdents.empty());

	std::sort(std::begin(uniqueIdents), std::end(uniqueIdents));
	auto iter = std::adjacent_find(std::begin(uniqueIdents), std::end(uniqueIdents));
	
	EXPECT_EQ(std::end(uniqueIdents), iter);
}
