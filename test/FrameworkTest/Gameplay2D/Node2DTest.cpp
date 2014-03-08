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
#include <Pomdog/Gameplay2D/Node2D.hpp>

using namespace Pomdog;

TEST(Node2D, TrivialCase)
{
	auto objectContext = std::make_shared<GameObjectContext>();
	auto rootObject = std::make_shared<GameObject>(objectContext);
	
	auto rootNode = rootObject->AddComponent<Node2D>(rootObject);
	EXPECT_TRUE(rootObject->HasComponent<Node2D>());
	EXPECT_NE(nullptr, rootObject->Component<Node2D>());
	
	auto childObject = std::make_shared<GameObject>(objectContext);
	rootNode->AddChild(childObject);
	
	ASSERT_FALSE(rootNode->Children().empty());
	EXPECT_EQ(1, rootNode->Children().size());
	EXPECT_EQ(childObject, rootNode->Children().front());
	ASSERT_TRUE(rootNode->Children().front()->HasComponent<Node2D>());
	EXPECT_EQ(rootObject, rootNode->Children().front()->Component<Node2D>()->Parent().lock());
	
	rootObject->RemoveComponent<Node2D>();
	EXPECT_TRUE(childObject->HasComponent<Node2D>());
	EXPECT_TRUE(childObject->Component<Node2D>()->Parent().expired());
}

TEST(Node2D, DeleteChildObjectCase)
{
	auto objectContext = std::make_shared<GameObjectContext>();
	auto rootObject = std::make_shared<GameObject>(objectContext);
	
	auto rootNode = rootObject->AddComponent<Node2D>(rootObject);
	EXPECT_TRUE(rootObject->HasComponent<Node2D>());
	EXPECT_NE(nullptr, rootObject->Component<Node2D>());
	
	auto childObject = std::make_shared<GameObject>(objectContext);
	rootNode->AddChild(childObject);
	
	ASSERT_FALSE(rootNode->Children().empty());
	EXPECT_EQ(1, rootNode->Children().size());
	EXPECT_EQ(childObject, rootNode->Children().front());
	ASSERT_TRUE(rootNode->Children().front()->HasComponent<Node2D>());
	EXPECT_EQ(rootObject, rootNode->Children().front()->Component<Node2D>()->Parent().lock());
	
	std::weak_ptr<GameObject> weakChild = childObject;
	childObject.reset();
	
	EXPECT_FALSE(weakChild.expired());
	EXPECT_FALSE(rootNode->Children().empty());
}

TEST(Node2D, RemoveChildNodeComponentCase)
{
	auto objectContext = std::make_shared<GameObjectContext>();
	auto rootObject = std::make_shared<GameObject>(objectContext);
	
	auto rootNode = rootObject->AddComponent<Node2D>(rootObject);
	EXPECT_TRUE(rootObject->HasComponent<Node2D>());
	EXPECT_NE(nullptr, rootObject->Component<Node2D>());
	
	auto childObject = std::make_shared<GameObject>(objectContext);
	rootNode->AddChild(childObject);
	
	ASSERT_FALSE(rootNode->Children().empty());
	EXPECT_EQ(1, rootNode->Children().size());
	EXPECT_EQ(childObject, rootNode->Children().front());
	ASSERT_TRUE(childObject->HasComponent<Node2D>());
	EXPECT_EQ(rootObject, childObject->Component<Node2D>()->Parent().lock());
	
	childObject->RemoveComponent<Node2D>();
	
	ASSERT_FALSE(childObject->HasComponent<Node2D>());
	EXPECT_TRUE(rootNode->Children().empty());
}

TEST(Node2D, RemoveNode)
{
	auto objectContext = std::make_shared<GameObjectContext>();
	auto rootObject = std::make_shared<GameObject>(objectContext);

	auto rootNode = rootObject->AddComponent<Node2D>(rootObject);
	EXPECT_TRUE(rootObject->HasComponent<Node2D>());
	EXPECT_NE(nullptr, rootObject->Component<Node2D>());
	
	auto childObject = std::make_shared<GameObject>(objectContext);
	rootNode->AddChild(childObject);

	ASSERT_EQ(childObject, rootNode->Children().front());
	ASSERT_TRUE(childObject->HasComponent<Node2D>());
	ASSERT_EQ(rootObject, childObject->Component<Node2D>()->Parent().lock());
	
	rootNode->RemoveChild(childObject);
	ASSERT_TRUE(childObject->HasComponent<Node2D>());
	ASSERT_TRUE(childObject->Component<Node2D>()->Parent().expired());
	ASSERT_TRUE(rootNode->Children().empty());
}
