// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/ECS/Entity.hpp"
#include "Pomdog/Experimental/ECS/ComponentTypeIndex.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "catch.hpp"
#include <cstdint>
#include <memory>

using namespace Pomdog;

namespace {
class Transform final : public Component {
public:
    Vector3 Position;
};
class GraphicsComponent : public Component {
public:
    int DrawOrder;
};
class Renderable final : public GraphicsComponent {
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
struct ComponentTypeDeclaration<GraphicsComponent> final {
    static std::uint8_t GetTypeIndex()
    {
        return Detail::Gameplay::ComponentTypeIndex::Index<GraphicsComponent>();
    }
};

template <>
struct ComponentTypeDeclaration<Renderable> final {
    static std::uint8_t GetTypeIndex()
    {
        return Detail::Gameplay::ComponentTypeIndex::Index<GraphicsComponent>();
    }
};

template <>
class ComponentCreator<Renderable> final : public ComponentCreatorBase {
public:
    std::shared_ptr<Component> CreateComponent() override
    {
        return std::make_shared<Renderable>();
    }

    std::uint8_t GetComponentType() override
    {
        return ComponentTypeDeclaration<Renderable>::GetTypeIndex();
    }
};

} // namespace Pomdog

TEST_CASE("Entity AddComponent", "[Entity]")
{
    EntityContext context;
    Entity entity{&context, context.Create({
        AddComponent<Transform>(),
        AddComponent<Renderable>()
    })};

    REQUIRE(entity.HasComponent<Transform>());
    REQUIRE(entity.GetComponent<Transform>());

    REQUIRE(entity.HasComponent<Renderable>());
    REQUIRE(entity.GetComponent<Renderable>());

    entity.GetComponent<Transform>()->Position = Vector3{3.0f, 4.0f, 5.0f};

    REQUIRE(entity.GetComponent<Transform>()->Position == Vector3{3.0f, 4.0f, 5.0f});
}

TEST_CASE("Entity AddComponent_WithInheritance", "[Entity]")
{
    EntityContext context;
    Entity entity{&context, context.Create({
        AddComponent<Transform>(),
        AddComponent<Renderable>()
    })};

    REQUIRE(entity.HasComponent<Renderable>());
    REQUIRE(entity.HasComponent<GraphicsComponent>());

    REQUIRE(entity.GetComponent<Renderable>());
    REQUIRE(entity.GetComponent<GraphicsComponent>());

    auto text = entity.GetComponent<Renderable>();
    text->DrawOrder = 42;
    REQUIRE(entity.GetComponent<Renderable>()->DrawOrder == 42);
    REQUIRE(entity.GetComponent<GraphicsComponent>()->DrawOrder == 42);

    auto graphicsComponent = entity.GetComponent<GraphicsComponent>();
    graphicsComponent->DrawOrder = 73;
    REQUIRE(entity.GetComponent<Renderable>()->DrawOrder == 73);
    REQUIRE(entity.GetComponent<GraphicsComponent>()->DrawOrder == 73);

    REQUIRE(std::dynamic_pointer_cast<Renderable>(graphicsComponent));
    REQUIRE(std::dynamic_pointer_cast<Renderable>(graphicsComponent) == text);
    REQUIRE(std::dynamic_pointer_cast<Renderable>(graphicsComponent)->DrawOrder == 73);
}

TEST_CASE("Entity Component_Const", "[Entity]")
{
    EntityContext context;
    auto entity = std::make_shared<Entity>(&context, context.Create({
        AddComponent<Transform>()
    }));

    REQUIRE(entity->HasComponent<Transform>());
    REQUIRE(entity->GetComponent<Transform>() != nullptr);

    entity->GetComponent<Transform>()->Position.X = 42.0f;
    REQUIRE(entity->GetComponent<Transform>()->Position.X == 42.0f);

    {
        std::shared_ptr<Entity const> entityConstRef = entity;
        REQUIRE(entityConstRef->HasComponent<Transform>());
        REQUIRE(entityConstRef->GetComponent<Transform>() != nullptr);
        REQUIRE(entityConstRef->GetComponent<Transform>()->Position.X == 42.0f);
    }
}

namespace {

struct Behavior final : public Pomdog::Component {
    void Do(Pomdog::Entity & self)
    {
        REQUIRE(self);
        REQUIRE(self.HasComponent<Behavior>());
        self.DestroyImmediate();
        REQUIRE_FALSE(self);
        //REQUIRE_FALSE(self.HasComponent<Behavior>());
    }

    std::shared_ptr<int> ptr = std::make_shared<int>(42);
};

} // unnamed namespace

namespace Pomdog {

template <>
struct ComponentTypeDeclaration<Behavior> final {
    static std::uint8_t GetTypeIndex()
    {
        return Detail::Gameplay::ComponentTypeIndex::Index<Behavior>();
    }
};

template <>
class ComponentCreator<Behavior> final : public ComponentCreatorBase {
public:
    std::shared_ptr<Component> CreateComponent() override
    {
        auto component = std::make_shared<Behavior>();
        return component;
    }

    std::uint8_t GetComponentType() override
    {
        return ComponentTypeDeclaration<Behavior>::GetTypeIndex();
    }
};

} // namespace Pomdog

TEST_CASE("Entity DestroyImmediate", "[Entity]")
{
    {
        EntityContext context;
        Entity entity{&context, context.Create({})};

        REQUIRE(entity);
        REQUIRE(context.GetCount() == 1);
        entity.DestroyImmediate();
        REQUIRE_FALSE(entity);
        REQUIRE(context.GetCount() == 0);
    }
    {
        EntityContext context;
        Entity entity{&context, context.Create({})};

        REQUIRE(entity);
        REQUIRE(context.GetCount() == 1);
        context.DestroyImmediate(entity.GetID());
        REQUIRE_FALSE(entity);
        REQUIRE(context.GetCount() == 0);
    }
    {
        EntityContext context;
        Entity entity1{&context, context.Create({})};
        Entity entity2{&context, context.Create({})};
        Entity entity3{&context, context.Create({})};

        REQUIRE(entity1);
        REQUIRE(entity2);
        REQUIRE(entity3);
        REQUIRE(context.GetCount() == 3);
        entity1.DestroyImmediate();
        REQUIRE_FALSE(entity1);
        REQUIRE(entity2);
        REQUIRE(entity3);
        REQUIRE(context.GetCount() == 2);
        entity3.DestroyImmediate();
        REQUIRE_FALSE(entity1);
        REQUIRE(entity2);
        REQUIRE_FALSE(entity3);
        REQUIRE(context.GetCount() == 1);
        entity2.DestroyImmediate();
        REQUIRE_FALSE(entity1);
        REQUIRE_FALSE(entity2);
        REQUIRE_FALSE(entity3);
        REQUIRE(context.GetCount() == 0);
    }

    {
        EntityContext context;
        Entity entity{&context, context.Create({
            AddComponent<Behavior>()
        })};
        auto behavior = entity.GetComponent<Behavior>();
        REQUIRE(behavior != nullptr);
        std::weak_ptr<int> weak = behavior->ptr;

        REQUIRE(entity);
        REQUIRE(entity.HasComponent<Behavior>());
        REQUIRE(entity.GetComponent<Behavior>() == behavior);
        REQUIRE_FALSE(weak.expired());
        REQUIRE(context.GetCount() == 1);
        behavior->Do(entity);
        REQUIRE_FALSE(entity);
        //REQUIRE_FALSE(entity.HasComponent<Behavior>());
        //REQUIRE_FALSE(entity.GetComponent<Behavior>());
        REQUIRE(behavior);
        REQUIRE_FALSE(weak.expired());
        behavior.reset();
        REQUIRE(weak.expired());
        REQUIRE(context.GetCount() == 0);
    }
}

TEST_CASE("Destroy", "[Entity]")
{
    {
        EntityContext context;
        Entity entity{&context, context.Create({})};

        REQUIRE(entity);
        REQUIRE(context.GetCount() == 1);
        entity.Destroy();
        REQUIRE_FALSE(entity);
        REQUIRE(context.GetCount() == 0);
        context.Refresh();
        REQUIRE_FALSE(entity);
        REQUIRE(context.GetCount() == 0);
    }
    {
        EntityContext context;
        Entity entity{&context, context.Create({})};

        REQUIRE(entity);
        REQUIRE(context.GetCount() == 1);
        context.Destroy(entity.GetID());
        REQUIRE_FALSE(entity);
        REQUIRE(context.GetCount() == 0);
        context.Refresh();
        REQUIRE_FALSE(entity);
        REQUIRE(context.GetCount() == 0);
    }
    {
        EntityContext context;
        Entity entity1{&context, context.Create({})};
        Entity entity2{&context, context.Create({})};
        Entity entity3{&context, context.Create({})};

        REQUIRE(entity1);
        REQUIRE(entity2);
        REQUIRE(entity3);
        REQUIRE(context.GetCount() == 3);
        entity1.Destroy();
        REQUIRE_FALSE(entity1);
        REQUIRE(entity2);
        REQUIRE(entity3);
        REQUIRE(context.GetCount() == 2);
        entity3.Destroy();
        REQUIRE_FALSE(entity1);
        REQUIRE(entity2);
        REQUIRE_FALSE(entity3);
        REQUIRE(context.GetCount() == 1);
        entity2.Destroy();
        REQUIRE_FALSE(entity1);
        REQUIRE_FALSE(entity2);
        REQUIRE_FALSE(entity3);
        REQUIRE(context.GetCount() == 0);
        context.Refresh();
        REQUIRE_FALSE(entity1);
        REQUIRE_FALSE(entity2);
        REQUIRE_FALSE(entity3);
        REQUIRE(context.GetCount() == 0);
    }

    {
        EntityContext context;
        Entity entity{&context, context.Create({
            AddComponent<Behavior>()
        })};
        auto behavior = entity.GetComponent<Behavior>();
        REQUIRE(behavior != nullptr);
        std::weak_ptr<int> weak = behavior->ptr;

        REQUIRE(entity);
        REQUIRE(entity.HasComponent<Behavior>());
        REQUIRE(entity.GetComponent<Behavior>() == behavior);
        REQUIRE_FALSE(weak.expired());
        REQUIRE(context.GetCount() == 1);
        behavior->Do(entity);
        REQUIRE_FALSE(entity);
        //REQUIRE_FALSE(entity.HasComponent<Behavior>());
        //REQUIRE(entity.GetComponent<Behavior>() == nullptr);
        REQUIRE_FALSE(weak.expired());
        REQUIRE(context.GetCount() == 0);
        context.Refresh();
        REQUIRE_FALSE(entity);
        //REQUIRE_FALSE(entity.HasComponent<Behavior>());
        //REQUIRE_FALSE(entity.GetComponent<Behavior>());
        REQUIRE_FALSE(weak.expired());
        REQUIRE(behavior);
        behavior.reset();
        REQUIRE(weak.expired());
        REQUIRE(context.GetCount() == 0);
    }
}

TEST_CASE("Clear", "[Entity]")
{
    {
        EntityContext context;
        Entity entity1{&context, context.Create({})};
        Entity entity2{&context, context.Create({})};
        Entity entity3{&context, context.Create({})};

        REQUIRE(entity1);
        REQUIRE(entity2);
        REQUIRE(entity3);
        REQUIRE(context.GetCount() == 3);
        context.Clear();
        REQUIRE_FALSE(entity1);
        REQUIRE_FALSE(entity2);
        REQUIRE_FALSE(entity3);
        REQUIRE(context.GetCount() == 0);
    }
    {
        EntityContext context;
        Entity entity1{&context, context.Create({
            AddComponent<Transform>(),
            AddComponent<Renderable>()
        })};
        Entity entity2{&context, context.Create({
            AddComponent<Transform>()
        })};
        Entity entity3{&context, context.Create({
            AddComponent<Renderable>()
        })};

        REQUIRE(entity1);
        REQUIRE(entity2);
        REQUIRE(entity3);
        REQUIRE(entity1.GetID() != entity2.GetID());
        REQUIRE(entity2.GetID() != entity3.GetID());
        REQUIRE(context.GetCount() == 3);
        context.Clear();
        REQUIRE_FALSE(entity1);
        REQUIRE_FALSE(entity2);
        REQUIRE_FALSE(entity3);
        REQUIRE(context.GetCount() == 0);
    }
    {
        EntityContext context;
        Entity entity{&context, context.Create({})};
        auto oldId = entity.GetID();

        REQUIRE(entity);
        REQUIRE(context.GetCount() == 1);
        context.Clear();
        REQUIRE_FALSE(entity);
        REQUIRE(context.GetCount() == 0);

        entity = Entity{&context, context.Create({})};
        auto newId = entity.GetID();
        REQUIRE(oldId != newId);
        REQUIRE(oldId.SequenceNumber() != newId.SequenceNumber());
        REQUIRE(oldId.Index() == newId.Index());
    }

    {
        EntityContext context;
        Entity entity{&context, context.Create({
            AddComponent<Behavior>()
        })};
        auto behavior = entity.GetComponent<Behavior>();
        REQUIRE(behavior != nullptr);
        std::weak_ptr<int> weak = behavior->ptr;

        REQUIRE(entity);
        REQUIRE(entity.HasComponent<Behavior>());
        REQUIRE(entity.GetComponent<Behavior>() == behavior);
        REQUIRE_FALSE(weak.expired());
        REQUIRE(context.GetCount() == 1);
        context.Clear();
        REQUIRE_FALSE(entity);
        REQUIRE(behavior);
        REQUIRE_FALSE(weak.expired());
        behavior.reset();
        REQUIRE(weak.expired());
        REQUIRE(context.GetCount() == 0);
    }
    {
        EntityContext context;
        Entity entity1{&context, context.Create({
            AddComponent<Behavior>()
        })};
        Entity entity2{&context, context.Create({})};
        auto behavior = entity1.GetComponent<Behavior>();
        REQUIRE(behavior != nullptr);
        std::weak_ptr<int> weak = behavior->ptr;

        auto oldId1 = entity1.GetID();
        auto oldId2 = entity2.GetID();

        REQUIRE(entity1);
        REQUIRE(entity2);
        REQUIRE(context.GetCount() == 2);
        context.Clear();
        REQUIRE_FALSE(entity1);
        REQUIRE_FALSE(entity2);
        REQUIRE(context.GetCount() == 0);

        entity1 = Entity{&context, context.Create({})};
        entity2 = Entity{&context, context.Create({})};

        REQUIRE(oldId1 != oldId2);
        REQUIRE(entity1.GetID() != oldId1);
        REQUIRE(entity2.GetID() != oldId2);
        REQUIRE(entity1.GetID().SequenceNumber() != oldId1.SequenceNumber());
        REQUIRE(entity2.GetID().SequenceNumber() != oldId2.SequenceNumber());
        REQUIRE(entity1.GetID().Index() == oldId1.Index());
        REQUIRE(entity2.GetID().Index() == oldId2.Index());
    }
}

TEST_CASE("Entity::GetID", "[Entity]")
{
    EntityContext context;
    Entity entity{&context, context.Create({})};

    auto id = entity.GetID();
    REQUIRE(entity.GetID() == id);

    Entity entity2{&context, context.Create({})};

    REQUIRE(entity.GetID() != entity2.GetID());
    REQUIRE(entity.GetID().Index() == 0);
    REQUIRE(entity2.GetID().Index() == 1);
    REQUIRE(entity.GetID().SequenceNumber() == 1);
    REQUIRE(entity2.GetID().SequenceNumber() == 1);
}

TEST_CASE("EntityID Sequence", "[Entity]")
{
    EntityContext context;
    {
        Entity entity{&context, context.Create({})};
        REQUIRE(entity.GetID().Index() == 0U);
        REQUIRE(entity.GetID().SequenceNumber() != 0U);
        REQUIRE(entity.GetID().SequenceNumber() == 1U);
    }
    {
        Entity entity{&context, context.Create({})};
        REQUIRE(entity.GetID().Index() == 1U);
        REQUIRE(entity.GetID().SequenceNumber() != 0U);
        REQUIRE(entity.GetID().SequenceNumber() == 1U);
        entity.DestroyImmediate();
    }
    {
        Entity entity{&context, context.Create({})};
        REQUIRE(entity.GetID().Index() == 1U);
        REQUIRE(entity.GetID().SequenceNumber() != 0U);
        REQUIRE(entity.GetID().SequenceNumber() == 2U);
        entity.DestroyImmediate();
    }
    {
        Entity entity{&context, context.Create({})};
        REQUIRE(entity.GetID().Index() == 1U);
        REQUIRE(entity.GetID().SequenceNumber() != 0U);
        REQUIRE(entity.GetID().SequenceNumber() == 3U);
        entity.DestroyImmediate();
    }
    {
        Entity entity1{&context, context.Create({})};
        REQUIRE(entity1.GetID().Index() == 1U);
        REQUIRE(entity1.GetID().SequenceNumber() != 0U);
        REQUIRE(entity1.GetID().SequenceNumber() == 4U);

        Entity entity2{&context, context.Create({})};
        REQUIRE(entity2.GetID().Index() == 2U);
        REQUIRE(entity2.GetID().SequenceNumber() != 0U);
        REQUIRE(entity2.GetID().SequenceNumber() == 1U);

        Entity entity3{&context, context.Create({})};
        REQUIRE(entity3.GetID().Index() == 3U);
        REQUIRE(entity3.GetID().SequenceNumber() != 0U);
        REQUIRE(entity3.GetID().SequenceNumber() == 1U);

        entity1.DestroyImmediate();
        entity2.DestroyImmediate();
        entity3.DestroyImmediate();
    }
}

TEST_CASE("Entity Cast Bool", "[Entity]")
{
    EntityContext context;
    {
        Entity entity{&context, context.Create({})};
        REQUIRE(entity);
        entity.DestroyImmediate();
        REQUIRE_FALSE(entity);
    }
    {
        Entity entity1{&context, context.Create({})};
        Entity entity2{&context, context.Create({})};
        REQUIRE(entity1);
        REQUIRE(entity2);
        entity1.DestroyImmediate();
        REQUIRE_FALSE(entity1);
        REQUIRE(entity2);
        entity2.DestroyImmediate();
        REQUIRE_FALSE(entity1);
        REQUIRE_FALSE(entity2);
    }
    {
        Entity entity{&context, context.Create({})};
        auto copiedObject = entity;
        REQUIRE(entity);
        REQUIRE(copiedObject);
        entity.DestroyImmediate();
        REQUIRE_FALSE(entity);
        REQUIRE_FALSE(copiedObject);
    }
    {
        Entity entity{&context, context.Create({})};
        auto copiedObject = entity;
        REQUIRE(entity);
        REQUIRE(copiedObject);
        copiedObject.DestroyImmediate();
        REQUIRE_FALSE(entity);
        REQUIRE_FALSE(copiedObject);
    }
}

TEST_CASE("Entity EqualOperator", "[Entity]")
{
    EntityContext context;
    Entity entity1{&context, context.Create({})};
    Entity entity2{&context, context.Create({})};
    REQUIRE(entity1 != entity2);
    REQUIRE(entity1 == entity1);
    REQUIRE(entity2 == entity2);

    Entity entity3 = entity1;
    REQUIRE(entity3 == entity1);
    REQUIRE(entity3 != entity2);

    entity3.DestroyImmediate();
    REQUIRE(entity3 != entity1);
    REQUIRE(entity3 != entity2);

    entity1.DestroyImmediate();
    REQUIRE(entity1 == entity3);
    REQUIRE(entity1 != entity2);

    entity2.DestroyImmediate();
    REQUIRE(entity2 == entity1);
    REQUIRE(entity2 == entity3);
}

TEST_CASE("Entity EntityID Unique", "[Entity]")
{
    std::vector<Entity> objects;
    std::vector<EntityID> uniqueIdents;

    EntityContext context;

    std::mt19937 random(10000);

    std::uint32_t maxSequenceNumber = 0;
    std::uint32_t maxIndex = 0;

    for (std::size_t count = 0; count < 262144; ++count)
    {
        if (count % 3 != 0)
        {
            Entity entity{&context, context.Create({})};
            objects.push_back(entity);
            uniqueIdents.push_back(entity.GetID());

            //printf("### Create Object: %llu(%u, %u) \n",
            //    entity.GetID().Value(),
            //    entity.GetID().SequenceNumber(),
            //    entity.GetID().Index());

            maxSequenceNumber = std::max(maxSequenceNumber, entity.GetID().SequenceNumber());
            maxIndex = std::max(maxIndex, entity.GetID().Index());
        }
        if (count % 11 == 8)
        {
            if (!objects.empty()) {
                REQUIRE(objects.front());
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

                for (auto & object: objects) {
                    REQUIRE(object);
                    if (object.GetID().Value() % randomNumber == 0) {
                        object.DestroyImmediate();
                    }
                }
                objects.erase(std::remove_if(std::begin(objects), std::end(objects),
                    [&](Entity const& object) { return !object; }), std::end(objects));

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
