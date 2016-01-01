// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include <Pomdog.Experimental/Gameplay/EntityManager.hpp>
#include <Pomdog.Experimental/Gameplay/Entity.hpp>
#include <gtest/iutest_switch.hpp>
#include <cstdint>

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

TEST(EntityManager, CreateObject)
{
    EntityManager manager;
    {
        auto entity = manager.CreateObject();
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<PhysicsComponent>();
    }
    {
        auto entity = manager.CreateObject();
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<PhysicsComponent>();
    }
    {
        auto entity = manager.CreateObject();
        entity.AddComponent<PhysicsComponent>();
    }
    {
        auto entity = manager.CreateObject();
    }
    {
        auto entitys = manager.QueryComponents<TransformComponent, PhysicsComponent>();
        EXPECT_EQ(2, entitys.size());
    }
    {
        auto entitys = manager.QueryComponents<PhysicsComponent, TransformComponent>();
        EXPECT_EQ(2, entitys.size());
    }
    {
        auto entitys = manager.QueryComponents<PhysicsComponent>();
        EXPECT_EQ(3, entitys.size());
    }
    {
        auto entitys = manager.QueryComponents<TransformComponent>();
        EXPECT_EQ(2, entitys.size());

        for (auto & objects: entitys) {
            objects.DestroyImmediate();
        }

        entitys = manager.QueryComponents<TransformComponent>();
        EXPECT_TRUE(entitys.empty());
    }
    {
        auto entitys = manager.QueryComponents<TransformComponent, PhysicsComponent>();
        EXPECT_TRUE(entitys.empty());
    }
    {
        auto entitys = manager.QueryComponents<PhysicsComponent, TransformComponent>();
        EXPECT_TRUE(entitys.empty());
    }
    {
        auto entitys = manager.QueryComponents<PhysicsComponent>();
        EXPECT_EQ(1, entitys.size());
    }
}

//TEST(EntityManager, AddChild)
//{
//    EntityManager manager;
//
//    auto entity = manager.CreateObject();
//    entity.AddComponent<TransformComponent>();
//    entity.AddComponent<PhysicsComponent>();
//
//    EXPECT_FALSE(entity->HasComponent<Texture3D>());
//
//    {
//        auto entity2 = manager.CreateObject();
//        entity2->AddComponent<Transform>();
//    }
//    {
//        auto entity3 = manager.CreateObject();
//        entity3->AddComponent<Collider>();
//    }
//    {
//        auto entity4 = manager.CreateObject();
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
//        auto entity = manager.CreateObject();
//        entity->AddComponent<Transform>(Transform{0, 0});
//        entity->AddComponent<Collider>();
//    }
//    {
//        auto entity = manager.CreateObject();
//        entity->AddComponent<Transform>();
//    }
//    {
//        auto entity = manager.CreateObject();
//        entity->AddComponent<Collider>();
//    }
//    {
//        auto entity = manager.CreateObject();
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
