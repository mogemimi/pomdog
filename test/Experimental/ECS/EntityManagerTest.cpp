// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/ECS/EntityManager.hpp"
#include "Pomdog/Experimental/ECS/ComponentTypeIndex.hpp"
#include "Pomdog/Experimental/ECS/Entity.hpp"
#include "Pomdog/Experimental/ECS/EntityArchtype.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "catch.hpp"
#include <cstdint>
#include <memory>

using Pomdog::Vector3;
using Pomdog::ECS::AddComponent;
using Pomdog::ECS::Entity;
using Pomdog::ECS::EntityArchtype;
using Pomdog::ECS::EntityManager;

namespace {

class Transform final {
public:
    Vector3 Position;
};

class Renderable {
public:
    int DrawOrder;
};

struct Behavior final {
    void Do(EntityManager& manager, Entity& self)
    {
        REQUIRE(manager.Exists(self));
        REQUIRE(manager.HasComponent<Behavior>(self));
        manager.DestroyEntity(self);
        REQUIRE_FALSE(manager.Exists(self));
    }

    std::shared_ptr<int> ptr = std::make_shared<int>(42);
};

template <typename T, typename... Args>
int ComputeCount(EntityManager& entities)
{
    int count = 0;
    entities.WithAll<T, Args...>().ForEach([&](auto&, auto&...) {
        count++;
    });
    return count;
}

} // namespace

TEST_CASE("EntityManager::CreateEntity", "[EntityManager]")
{
    EntityManager manager;
    {
        auto archtype = EntityArchtype{};
        auto entity = manager.CreateEntity(archtype);
        REQUIRE(manager.Exists(entity));
        REQUIRE_FALSE(manager.HasComponent<Transform>(entity));
        REQUIRE_FALSE(manager.HasComponent<Renderable>(entity));
    }
    {
        auto archtype = EntityArchtype{{
            AddComponent<Transform>()
        }};
        auto entity = manager.CreateEntity(archtype);
        REQUIRE(manager.Exists(entity));
        REQUIRE(manager.HasComponent<Transform>(entity));
        REQUIRE_FALSE(manager.HasComponent<Renderable>(entity));
    }
    {
        auto archtype = EntityArchtype{{
            AddComponent<Renderable>()
        }};
        auto entity = manager.CreateEntity(archtype);
        REQUIRE(manager.Exists(entity));
        REQUIRE_FALSE(manager.HasComponent<Transform>(entity));
        REQUIRE(manager.HasComponent<Renderable>(entity));
    }
    {
        auto archtype = EntityArchtype{{
            AddComponent<Transform>(),
            AddComponent<Renderable>()
        }};
        auto entity = manager.CreateEntity(archtype);
        REQUIRE(manager.Exists(entity));
        REQUIRE(manager.HasComponent<Transform>(entity));
        REQUIRE(manager.HasComponent<Renderable>(entity));
    }
    {
        REQUIRE(ComputeCount<Transform, Renderable>(manager) == 1);
        REQUIRE(ComputeCount<Renderable, Transform>(manager) == 1);
        REQUIRE(ComputeCount<Transform>(manager) == 2);
        REQUIRE(ComputeCount<Renderable>(manager) == 2);
    }
    {
        REQUIRE(ComputeCount<Transform>(manager) == 2);

        manager.WithAll<Entity, Transform>().ForEach([&](const Entity& entity, auto&) {
            manager.DestroyEntity(entity);
        });

        REQUIRE(ComputeCount<Transform>(manager) == 0);
        REQUIRE(ComputeCount<Transform, Renderable>(manager) == 0);
        REQUIRE(ComputeCount<Renderable, Transform>(manager) == 0);
        REQUIRE(ComputeCount<Renderable>(manager) == 1);
    }
}

TEST_CASE("Entity AddComponent", "[EntityManager]")
{
    EntityManager manager;

    auto archtype = EntityArchtype{{
        AddComponent<Transform>(),
        AddComponent<Renderable>()
    }};
    auto entity = manager.CreateEntity(archtype);

    REQUIRE(manager.HasComponent<Transform>(entity));
    REQUIRE(manager.GetComponent<Transform>(entity) != nullptr);

    REQUIRE(manager.HasComponent<Renderable>(entity));
    REQUIRE(manager.GetComponent<Renderable>(entity) != nullptr);

    auto transform = manager.GetComponent<Transform>(entity);
    transform->Position = Vector3{3.0f, 4.0f, 5.0f};

    auto transform2 = manager.GetComponent<Transform>(entity);
    REQUIRE(transform2 != nullptr);
    REQUIRE(transform2->Position == Vector3{3.0f, 4.0f, 5.0f});
}

TEST_CASE("Destroy", "[EntityManager]")
{
    EntityManager manager;

    auto archtype = EntityArchtype{{
        AddComponent<Behavior>()
    }};

    SECTION("") {
        auto entity = manager.CreateEntity(archtype);

        REQUIRE(manager.Exists(entity));
        REQUIRE(manager.GetCount() == 1);
        manager.DestroyEntity(entity);
        REQUIRE_FALSE(manager.Exists(entity));
        REQUIRE(manager.GetCount() == 0);
    }
    SECTION("") {
        auto entity = manager.CreateEntity(archtype);

        REQUIRE(manager.Exists(entity));
        REQUIRE(manager.GetCount() == 1);
        manager.DestroyEntity(entity);
        REQUIRE_FALSE(manager.Exists(entity));
        REQUIRE(manager.GetCount() == 0);
    }
    SECTION("") {
        auto entity1 = manager.CreateEntity(archtype);
        auto entity2 = manager.CreateEntity(archtype);
        auto entity3 = manager.CreateEntity(archtype);

        REQUIRE(manager.Exists(entity1));
        REQUIRE(manager.Exists(entity2));
        REQUIRE(manager.Exists(entity3));
        REQUIRE(manager.GetCount() == 3);
        manager.DestroyEntity(entity1);
        REQUIRE_FALSE(manager.Exists(entity1));
        REQUIRE(manager.Exists(entity2));
        REQUIRE(manager.Exists(entity3));
        REQUIRE(manager.GetCount() == 2);
        manager.DestroyEntity(entity3);
        REQUIRE_FALSE(manager.Exists(entity1));
        REQUIRE(manager.Exists(entity2));
        REQUIRE_FALSE(manager.Exists(entity3));
        REQUIRE(manager.GetCount() == 1);
        manager.DestroyEntity(entity2);
        REQUIRE_FALSE(manager.Exists(entity1));
        REQUIRE_FALSE(manager.Exists(entity2));
        REQUIRE_FALSE(manager.Exists(entity3));
        REQUIRE(manager.GetCount() == 0);
    }
}

TEST_CASE("Destroy_Behavior", "[EntityManager]")
{
    EntityManager manager;

    auto archtype = EntityArchtype{{
        AddComponent<Behavior>()
    }};

    SECTION("") {
        auto entity = manager.CreateEntity(archtype);

        auto behavior = manager.GetComponent<Behavior>(entity);
        REQUIRE(behavior != nullptr);
        std::weak_ptr<int> weak = behavior->ptr;

        REQUIRE(manager.Exists(entity));
        REQUIRE(manager.HasComponent<Behavior>(entity));
        REQUIRE(manager.GetComponent<Behavior>(entity) == behavior);
        REQUIRE_FALSE(weak.expired());
        REQUIRE(manager.GetCount() == 1);
        behavior->Do(manager, entity);
        REQUIRE_FALSE(manager.Exists(entity));
        REQUIRE_FALSE(manager.HasComponent<Behavior>(entity));
        REQUIRE(manager.GetComponent<Behavior>(entity) == nullptr);
        REQUIRE(weak.expired());
        REQUIRE(manager.GetCount() == 0);
    }
}

TEST_CASE("DestroyAllEntities", "[EntityManager]")
{
    EntityManager manager;

    SECTION("") {
        auto entity1 = manager.CreateEntity({});
        auto entity2 = manager.CreateEntity({});
        auto entity3 = manager.CreateEntity({});

        REQUIRE(manager.Exists(entity1));
        REQUIRE(manager.Exists(entity2));
        REQUIRE(manager.Exists(entity3));
        REQUIRE(manager.GetCount() == 3);
        manager.DestroyAllEntities();
        REQUIRE_FALSE(manager.Exists(entity1));
        REQUIRE_FALSE(manager.Exists(entity2));
        REQUIRE_FALSE(manager.Exists(entity3));
        REQUIRE(manager.GetCount() == 0);
    }
    SECTION("") {
        auto archtype1 = EntityArchtype{{
            AddComponent<Transform>(),
            AddComponent<Renderable>()
        }};
        auto archtype2 = EntityArchtype{{
            AddComponent<Transform>()
        }};
        auto archtype3 = EntityArchtype{{
            AddComponent<Renderable>()
        }};
        auto entity1 = manager.CreateEntity(archtype1);
        auto entity2 = manager.CreateEntity(archtype2);
        auto entity3 = manager.CreateEntity(archtype3);

        REQUIRE(manager.Exists(entity1));
        REQUIRE(manager.Exists(entity2));
        REQUIRE(manager.Exists(entity3));
        REQUIRE(entity1 != entity2);
        REQUIRE(entity2 != entity3);
        REQUIRE(manager.GetCount() == 3);
        manager.DestroyAllEntities();
        REQUIRE_FALSE(manager.Exists(entity1));
        REQUIRE_FALSE(manager.Exists(entity2));
        REQUIRE_FALSE(manager.Exists(entity3));
        REQUIRE(manager.GetCount() == 0);
    }
    SECTION("") {
        auto entity = manager.CreateEntity({});
        auto oldId = entity;

        REQUIRE(manager.Exists(entity));
        REQUIRE(manager.GetCount() == 1);
        manager.DestroyAllEntities();
        REQUIRE_FALSE(manager.Exists(entity));
        REQUIRE(manager.GetCount() == 0);

        entity = manager.CreateEntity({});
        auto newId = entity;
        REQUIRE(oldId != newId);
        REQUIRE(oldId.GetVersion() != newId.GetVersion());
        REQUIRE(oldId.GetIndex() == newId.GetIndex());
    }
    SECTION("") {
        auto archtype = EntityArchtype{{
            AddComponent<Behavior>()
        }};
        auto entity = manager.CreateEntity(archtype);

        auto behavior = manager.GetComponent<Behavior>(entity);
        REQUIRE(behavior != nullptr);
        std::weak_ptr<int> weak = behavior->ptr;

        REQUIRE(manager.Exists(entity));
        REQUIRE(manager.HasComponent<Behavior>(entity));
        REQUIRE(manager.GetComponent<Behavior>(entity) == behavior);
        REQUIRE_FALSE(weak.expired());
        REQUIRE(manager.GetCount() == 1);
        manager.DestroyAllEntities();
        REQUIRE_FALSE(manager.Exists(entity));
        REQUIRE(weak.expired());
        REQUIRE(manager.GetCount() == 0);
    }
    SECTION("") {
        auto archtype1 = EntityArchtype{{
            AddComponent<Behavior>()
        }};
        auto archtype2 = EntityArchtype{{
        }};
        auto entity1 = manager.CreateEntity(archtype1);
        auto entity2 = manager.CreateEntity(archtype2);

        auto behavior = manager.GetComponent<Behavior>(entity1);
        REQUIRE(behavior != nullptr);
        std::weak_ptr<int> weak = behavior->ptr;

        auto oldId1 = entity1;
        auto oldId2 = entity2;

        REQUIRE(manager.Exists(entity1));
        REQUIRE(manager.Exists(entity2));
        REQUIRE(manager.GetCount() == 2);
        manager.DestroyAllEntities();
        REQUIRE_FALSE(manager.Exists(entity1));
        REQUIRE_FALSE(manager.Exists(entity2));
        REQUIRE(manager.GetCount() == 0);

        entity1 = manager.CreateEntity({});
        entity2 = manager.CreateEntity({});

        REQUIRE(oldId1 != oldId2);
        REQUIRE(entity1 != oldId1);
        REQUIRE(entity2 != oldId2);
        REQUIRE(entity1.GetVersion() != oldId1.GetVersion());
        REQUIRE(entity2.GetVersion() != oldId2.GetVersion());
        REQUIRE(entity1.GetIndex() == oldId1.GetIndex());
        REQUIRE(entity2.GetIndex() == oldId2.GetIndex());
    }
}

TEST_CASE("Entity::GetID", "[EntityManager]")
{
    EntityManager manager;
    auto entity = manager.CreateEntity({});

    auto id = entity;
    REQUIRE(entity == id);

    auto entity2 = manager.CreateEntity({});

    REQUIRE(entity != entity2);
    REQUIRE(entity.GetIndex() == 0);
    REQUIRE(entity2.GetIndex() == 1);
    REQUIRE(entity.GetVersion() == 1);
    REQUIRE(entity2.GetVersion() == 1);
}

TEST_CASE("EntityID Sequence", "[EntityManager]")
{
    EntityManager manager;
    {
        auto entity = manager.CreateEntity({});
        REQUIRE(entity.GetIndex() == 0U);
        REQUIRE(entity.GetVersion() != 0U);
        REQUIRE(entity.GetVersion() == 1U);
    }
    {
        auto entity = manager.CreateEntity({});
        REQUIRE(entity.GetIndex() == 1U);
        REQUIRE(entity.GetVersion() != 0U);
        REQUIRE(entity.GetVersion() == 1U);
        manager.DestroyEntity(entity);
    }
    {
        auto entity = manager.CreateEntity({});
        REQUIRE(entity.GetIndex() == 1U);
        REQUIRE(entity.GetVersion() != 0U);
        REQUIRE(entity.GetVersion() == 2U);
        manager.DestroyEntity(entity);
    }
    {
        auto entity = manager.CreateEntity({});
        REQUIRE(entity.GetIndex() == 1U);
        REQUIRE(entity.GetVersion() != 0U);
        REQUIRE(entity.GetVersion() == 3U);
        manager.DestroyEntity(entity);
    }
    {
        auto entity1 = manager.CreateEntity({});
        REQUIRE(entity1.GetIndex() == 1U);
        REQUIRE(entity1.GetVersion() != 0U);
        REQUIRE(entity1.GetVersion() == 4U);

        auto entity2 = manager.CreateEntity({});
        REQUIRE(entity2.GetIndex() == 2U);
        REQUIRE(entity2.GetVersion() != 0U);
        REQUIRE(entity2.GetVersion() == 1U);

        auto entity3 = manager.CreateEntity({});
        REQUIRE(entity3.GetIndex() == 3U);
        REQUIRE(entity3.GetVersion() != 0U);
        REQUIRE(entity3.GetVersion() == 1U);

        manager.DestroyEntity(entity1);
        manager.DestroyEntity(entity2);
        manager.DestroyEntity(entity3);
    }
}

TEST_CASE("Entity Cast Bool", "[EntityManager]")
{
    EntityManager manager;
    {
        auto entity = manager.CreateEntity({});
        REQUIRE(manager.Exists(entity));
        manager.DestroyEntity(entity);
        REQUIRE_FALSE(manager.Exists(entity));
    }
    {
        auto entity1 = manager.CreateEntity({});
        auto entity2 = manager.CreateEntity({});
        REQUIRE(manager.Exists(entity1));
        REQUIRE(manager.Exists(entity2));
        manager.DestroyEntity(entity1);
        REQUIRE_FALSE(manager.Exists(entity1));
        REQUIRE(manager.Exists(entity2));
        manager.DestroyEntity(entity2);
        REQUIRE_FALSE(manager.Exists(entity1));
        REQUIRE_FALSE(manager.Exists(entity2));
    }
    {
        auto entity = manager.CreateEntity({});
        auto copied = entity;
        REQUIRE(manager.Exists(entity));
        REQUIRE(manager.Exists(copied));
        manager.DestroyEntity(entity);
        REQUIRE_FALSE(manager.Exists(entity));
        REQUIRE_FALSE(manager.Exists(copied));
    }
    {
        auto entity = manager.CreateEntity({});
        auto copied = entity;
        REQUIRE(manager.Exists(entity));
        REQUIRE(manager.Exists(copied));
        manager.DestroyEntity(copied);
        REQUIRE_FALSE(manager.Exists(entity));
        REQUIRE_FALSE(manager.Exists(copied));
    }
}

TEST_CASE("Entity EqualOperator", "[EntityManager]")
{
    EntityManager manager;
    auto entity1 = manager.CreateEntity({});
    auto entity2 = manager.CreateEntity({});
    REQUIRE(entity1 != entity2);
    REQUIRE(entity1 == entity1);
    REQUIRE(entity2 == entity2);

    Entity entity3 = entity1;
    REQUIRE(entity3 == entity1);
    REQUIRE(entity3 != entity2);

    manager.DestroyEntity(entity3);
    REQUIRE(entity3 == entity1);
    REQUIRE(entity3 != entity2);
    REQUIRE(entity1 != entity2);

    manager.DestroyEntity(entity2);
    REQUIRE(entity2 != entity1);
    REQUIRE(entity2 != entity3);
}

TEST_CASE("Entity EntityID Unique", "[EntityManager]")
{
    std::vector<Entity> objects;
    std::vector<Entity> uniqueIdents;

    EntityManager manager;

    std::mt19937 random(10000);

    std::uint32_t maxSequenceNumber = 0;
    std::uint32_t maxIndex = 0;

    for (std::size_t count = 0; count < 262144; ++count) {
        if (count % 3 != 0) {
            auto entity = manager.CreateEntity({});
            objects.push_back(entity);
            uniqueIdents.push_back(entity);

            //printf("### Create Object: %llu(%u, %u) \n",
            //    entity.Value(),
            //    entity.SequenceNumber(),
            //    entity.Index());

            maxSequenceNumber = std::max(maxSequenceNumber, entity.GetVersion());
            maxIndex = std::max(maxIndex, entity.GetIndex());
        }
        if (count % 11 == 8) {
            if (!objects.empty()) {
                auto entity = objects.front();
                REQUIRE(manager.Exists(entity));
                manager.DestroyEntity(entity);
                objects.erase(objects.begin());
                //printf("### Remove Object \n");
            }
        }
        if (count % 14 == 9) {
            if (!objects.empty()) {
                std::uniform_int_distribution<std::uint32_t> distribution(1, 13);
                auto const randomNumber = distribution(random);

                for (auto& entity : objects) {
                    REQUIRE(manager.Exists(entity));
                    if (entity.GetUInt64Value() % randomNumber == 0) {
                        manager.DestroyEntity(entity);
                    }
                }
                objects.erase(std::remove_if(std::begin(objects), std::end(objects),
                    [&](const Entity& entity) { return !manager.Exists(entity); }), std::end(objects));

                //printf("### Remove Objects \n");
            }
        }
    }

    //printf("## maxSequenceNumber = %u\n## maxIndex = %u\n", maxSequenceNumber, maxIndex);

    REQUIRE_FALSE(uniqueIdents.empty());

    std::sort(std::begin(uniqueIdents), std::end(uniqueIdents));
    auto iter = std::adjacent_find(std::begin(uniqueIdents), std::end(uniqueIdents));
    REQUIRE(iter == std::end(uniqueIdents));
}

TEST_CASE("EntityManager::SetComponentData", "[EntityManager]")
{
    EntityManager manager;
    auto archtype = EntityArchtype{{
        AddComponent<Renderable>()
    }};
    auto entity = manager.CreateEntity(archtype);

    manager.SetComponentData<Renderable>(entity, Renderable{42});
    REQUIRE(manager.GetComponent<Renderable>(entity)->DrawOrder == 42);
}

TEST_CASE("EntityManager::WithAll", "[EntityManager]")
{
    EntityManager manager;

    auto archtype1 = EntityArchtype{{
        AddComponent<Transform>(),
        AddComponent<Renderable>()
    }};
    auto archtype2 = EntityArchtype{{
        AddComponent<Transform>()
    }};
    auto archtype3 = EntityArchtype{{
        AddComponent<Renderable>()
    }};

    for (int i = 0; i < 42; i++) {
        auto entity = manager.CreateEntity(archtype1);
        REQUIRE(manager.Exists(entity));
        REQUIRE(manager.HasComponent<Transform>(entity));
        REQUIRE(manager.HasComponent<Renderable>(entity));
    }
    for (int i = 0; i < 10; i++) {
        auto entity = manager.CreateEntity(archtype2);
        REQUIRE(manager.Exists(entity));
        REQUIRE(manager.HasComponent<Transform>(entity));
    }
    for (int i = 0; i < 3; i++) {
        auto entity = manager.CreateEntity(archtype3);
        REQUIRE(manager.Exists(entity));
        REQUIRE(manager.HasComponent<Renderable>(entity));
    }

    {
        int count = 0;
        manager.WithAll<Transform>().ForEach([&](auto&) {
            count++;
        });
        REQUIRE(count == (42 + 10));
    }
    {
        int count = 0;
        manager.WithAll<Renderable>().ForEach([&](auto&) {
            count++;
        });
        REQUIRE(count == (42 + 3));
    }
    {
        int count = 0;
        manager.WithAll<Transform, Renderable>().ForEach([&](auto&, auto&) {
            count++;
        });
        REQUIRE(count == 42);
    }
}
