// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/ECS/EntityManager.hpp"
#include "Pomdog/Experimental/ECS/Entity.hpp"
#include "Pomdog/Experimental/ECS/ComponentTypeIndex.hpp"
#include "catch.hpp"
#include <cstdint>

using namespace Pomdog;

namespace {
class Transform final : public Component {
};
class ActorComponent final : public Component {
};
} // namespace

namespace Pomdog {

template <>
struct ComponentTypeDeclaration<Transform> final {
    static std::uint8_t GetTypeIndex()
    {
        return Detail::Gameplay::ComponentTypeIndex::Index<Transform>();
    }
};

template <>
class ComponentCreator<Transform> final : public ComponentCreatorBase {
public:
    std::shared_ptr<Component> CreateComponent() override
    {
        return std::make_shared<Transform>();
    }

    std::uint8_t GetComponentType() override
    {
        return ComponentTypeDeclaration<Transform>::GetTypeIndex();
    }
};

template <>
struct ComponentTypeDeclaration<ActorComponent> final {
    static std::uint8_t GetTypeIndex()
    {
        return Detail::Gameplay::ComponentTypeIndex::Index<ActorComponent>();
    }
};

template <>
class ComponentCreator<ActorComponent> final : public ComponentCreatorBase {
public:
    std::shared_ptr<Component> CreateComponent() override
    {
        return std::make_shared<ActorComponent>();
    }

    std::uint8_t GetComponentType() override
    {
        return ComponentTypeDeclaration<ActorComponent>::GetTypeIndex();
    }
};

} // namespace Pomdog

TEST_CASE("EntityManager::CreateEntity", "[EntityManager]")
{
    EntityManager manager;
    {
        auto entity = manager.CreateEntity({});
        REQUIRE(entity);
        REQUIRE_FALSE(entity.HasComponent<Transform>());
        REQUIRE_FALSE(entity.HasComponent<ActorComponent>());
    }
    {
        auto entity = manager.CreateEntity({
            AddComponent<Transform>()
        });
        REQUIRE(entity);
        REQUIRE(entity.HasComponent<Transform>());
        REQUIRE_FALSE(entity.HasComponent<ActorComponent>());
    }
    {
        auto entity = manager.CreateEntity({
            AddComponent<ActorComponent>()
        });
        REQUIRE(entity);
        REQUIRE_FALSE(entity.HasComponent<Transform>());
        REQUIRE(entity.HasComponent<ActorComponent>());
    }
    {
        auto entity = manager.CreateEntity({
            AddComponent<Transform>(),
            AddComponent<ActorComponent>()
        });
        REQUIRE(entity);
        REQUIRE(entity.HasComponent<Transform>());
        REQUIRE(entity.HasComponent<ActorComponent>());
    }
    {
        auto entities = manager.QueryComponents<Transform, ActorComponent>();
        REQUIRE(entities.size() == 1);
    }
    {
        auto entities = manager.QueryComponents<Transform>();
        REQUIRE(entities.size() == 2);
    }
    {
        auto entities = manager.QueryComponents<ActorComponent>();
        REQUIRE(entities.size() == 2);
    }
    {
        auto entities = manager.QueryComponents<Transform>();
        REQUIRE(entities.size() == 2);

        for (auto & entity : entities) {
            entity.DestroyImmediate();
        }

        entities = manager.QueryComponents<Transform>();
        REQUIRE(entities.empty());
    }
    {
        auto entities = manager.QueryComponents<Transform, ActorComponent>();
        REQUIRE(entities.empty());
    }
    {
        auto entities = manager.QueryComponents<ActorComponent, Transform>();
        REQUIRE(entities.empty());
    }
    {
        auto entities = manager.QueryComponents<ActorComponent>();
        REQUIRE(entities.size() == 1);
    }
}

//TEST_CASE("AddChild", "[EntityManager]")
//{
//    EntityManager manager;
//
//    auto entity = manager.CreateEntity();
//    entity.AddComponent<TransformComponent>();
//    entity.AddComponent<PhysicsComponent>();
//
//    REQUIRE_FALSE(entity->HasComponent<Texture3D>());
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
//    REQUIRE(objects.size() == 1);
//
//    for (auto entity: objects)
//    {
//        auto transform = entity->GetComponent<Transform>();
//        auto collider = entity->GetComponent<Collider>();
//
//        REQUIRE(transform != nullptr);
//        transform->x = 480;
//        transform->y = 320;
//
//        REQUIRE(collider != nullptr);
//        collider->radius = 50;
//    }
//
//    REQUIRE_FALSE(nullptr, entity->GetComponent<Transform>());
//    REQUIRE(entity->GetComponent<Transform>()->x == 480);
//    REQUIRE(entity->GetComponent<Transform>()->y == 320);
//
//    REQUIRE_FALSE(nullptr, entity->GetComponent<Collider>());
//    REQUIRE(entity->GetComponent<Collider>()->radius == 50);
//}
//
//
//TEST_CASE("QueryComponents_Not", "[EntityManager]")
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
//    REQUIRE(objects.size() == 1);
//
//    for (auto entity: objects)
//    {
//        EXPECT_NE(nullptr, entity->GetComponent<Transform>());
//        REQUIRE(entity->HasComponent<Transform>());
//
//        REQUIRE(entity->GetComponent<Collider>() == nullptr);
//        REQUIRE_FALSE(entity->HasComponent<Collider>());
//    }
//}
