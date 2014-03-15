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
using Pomdog::GameObjectContext;

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
	auto objectContext = std::make_shared<GameObjectContext>();
	GameObject gameObject{ objectContext };
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
	auto objectContext = std::make_shared<GameObjectContext>();
	GameObject gameObject{ objectContext };
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
	auto objectContext = std::make_shared<GameObjectContext>();
	GameObject gameObject{ objectContext };
	gameObject.AddComponent<TransformInteger>();
	ASSERT_TRUE(gameObject.HasComponent<TransformInteger>());
	
	gameObject.RemoveComponent<TransformInteger>();
	EXPECT_FALSE(gameObject.HasComponent<TransformInteger>());
	EXPECT_EQ(nullptr, gameObject.Component<TransformInteger>());
}

TEST(GameObject, GameObjectID)
{
	auto objectContext = std::make_shared<GameObjectContext>();
	GameObject gameObject { objectContext };
	
	auto id = gameObject.ID();
	EXPECT_EQ(id, gameObject.ID());
	
	GameObject gameObject2 { objectContext };
	
	EXPECT_NE(gameObject.ID(), gameObject2.ID());
	EXPECT_EQ(0, gameObject.ID().Index());
	EXPECT_EQ(1, gameObject2.ID().Index());
	EXPECT_EQ(1, gameObject.ID().SequenceNumber());
	EXPECT_EQ(1, gameObject2.ID().SequenceNumber());
}

TEST(GameObject, Component_Const)
{
	auto objectContext = std::make_shared<GameObjectContext>();
	auto gameObject = std::make_shared<GameObject>(objectContext);
	
	EXPECT_FALSE(gameObject->HasComponent<int>());
	EXPECT_EQ(nullptr, gameObject->Component<int>());
	
	gameObject->AddComponent<int>(42);

	EXPECT_TRUE(gameObject->HasComponent<int>());
	ASSERT_NE(nullptr, gameObject->Component<int>());
	EXPECT_EQ(42, *gameObject->Component<int>());
	
	{
		std::shared_ptr<GameObject const> gameObjectConstRef = gameObject;
		EXPECT_TRUE(gameObjectConstRef->HasComponent<int>());
		ASSERT_NE(nullptr, gameObjectConstRef->Component<int>());
		EXPECT_EQ(42, *gameObjectConstRef->Component<int>());
	}
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
