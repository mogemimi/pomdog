// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog.Experimental/Gameplay/EntityManager.hpp>
#include <Pomdog.Experimental/Gameplay/Entity.hpp>
#include <Pomdog.Experimental/Gameplay2D/ActorComponent.hpp>
#include <Pomdog.Experimental/Gameplay2D/Transform.hpp>
#include <gtest/iutest_switch.hpp>
#include <cstdint>

using namespace Pomdog;

TEST(EntityManager, CreateEntity)
{
    EntityManager manager;
    {
        auto entity = manager.CreateEntity({});
        EXPECT_TRUE(entity);
        EXPECT_FALSE(entity.HasComponent<Transform>());
        EXPECT_FALSE(entity.HasComponent<ActorComponent>());
    }
    {
        auto entity = manager.CreateEntity({
            AddComponent<Transform>()
        });
        EXPECT_TRUE(entity);
        EXPECT_TRUE(entity.HasComponent<Transform>());
        EXPECT_FALSE(entity.HasComponent<ActorComponent>());
    }
    {
        auto entity = manager.CreateEntity({
            AddComponent<ActorComponent>()
        });
        EXPECT_TRUE(entity);
        EXPECT_FALSE(entity.HasComponent<Transform>());
        EXPECT_TRUE(entity.HasComponent<ActorComponent>());
    }
    {
        auto entity = manager.CreateEntity({
            AddComponent<Transform>(),
            AddComponent<ActorComponent>()
        });
        EXPECT_TRUE(entity);
        EXPECT_TRUE(entity.HasComponent<Transform>());
        EXPECT_TRUE(entity.HasComponent<ActorComponent>());
    }
    {
        auto entities = manager.QueryComponents<Transform, ActorComponent>();
        EXPECT_EQ(1, entities.size());
    }
    {
        auto entities = manager.QueryComponents<Transform>();
        EXPECT_EQ(2, entities.size());
    }
    {
        auto entities = manager.QueryComponents<ActorComponent>();
        EXPECT_EQ(2, entities.size());
    }
    {
        auto entities = manager.QueryComponents<Transform>();
        EXPECT_EQ(2, entities.size());

        for (auto & entity : entities) {
            entity.DestroyImmediate();
        }

        entities = manager.QueryComponents<Transform>();
        EXPECT_TRUE(entities.empty());
    }
    {
        auto entities = manager.QueryComponents<Transform, ActorComponent>();
        EXPECT_TRUE(entities.empty());
    }
    {
        auto entities = manager.QueryComponents<ActorComponent, Transform>();
        EXPECT_TRUE(entities.empty());
    }
    {
        auto entities = manager.QueryComponents<ActorComponent>();
        EXPECT_EQ(1, entities.size());
    }
}

//TEST(EntityManager, AddChild)
//{
//    EntityManager manager;
//
//    auto entity = manager.CreateEntity();
//    entity.AddComponent<TransformComponent>();
//    entity.AddComponent<PhysicsComponent>();
//
//    EXPECT_FALSE(entity->HasComponent<Texture3D>());
//
//    {
//        auto entity2 = manager.CreateEntity();
//        entity2->AddComponent<Transform>();
//    }
//    {
//        auto entity3 = manager.CreateEntity();
//        entity3->AddComponent<Collider>();
//    }
//    {
//        auto entity4 = manager.CreateEntity();
//    }
//
//    auto objects = manager.QueryComponents<Transform, Collider>();
//    EXPECT_EQ(1, objects.size());
//
//    for (auto entity: objects)
//    {
//        auto transform = entity->GetComponent<Transform>();
//        auto collider = entity->GetComponent<Collider>();
//
//        ASSERT_NE(nullptr, transform);
//        transform->x = 480;
//        transform->y = 320;
//
//        ASSERT_NE(nullptr, collider);
//        collider->radius = 50;
//    }
//
//    ASSERT_NE(nullptr, entity->GetComponent<Transform>());
//    EXPECT_EQ(480, entity->GetComponent<Transform>()->x);
//    EXPECT_EQ(320, entity->GetComponent<Transform>()->y);
//
//    ASSERT_NE(nullptr, entity->GetComponent<Collider>());
//    EXPECT_EQ(50, entity->GetComponent<Collider>()->radius);
//}
//
//
//TEST(EntityManager, QueryComponents_Not)
//{
//    EntityManager manager;
//    {
//        auto entity = manager.CreateEntity();
//        entity->AddComponent<Transform>(Transform{0, 0});
//        entity->AddComponent<Collider>();
//    }
//    {
//        auto entity = manager.CreateEntity();
//        entity->AddComponent<Transform>();
//    }
//    {
//        auto entity = manager.CreateEntity();
//        entity->AddComponent<Collider>();
//    }
//    {
//        auto entity = manager.CreateEntity();
//    }
//
//    auto objects = manager.QueryComponents<Transform, ComponentQuery::Not<Collider>>();
//    EXPECT_EQ(1, objects.size());
//
//    for (auto entity: objects)
//    {
//        EXPECT_NE(nullptr, entity->GetComponent<Transform>());
//        EXPECT_TRUE(entity->HasComponent<Transform>());
//
//        EXPECT_EQ(nullptr, entity->GetComponent<Collider>());
//        EXPECT_FALSE(entity->HasComponent<Collider>());
//    }
//}
