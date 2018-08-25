// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog.Experimental/Gameplay/Entity.hpp>
#include <Pomdog.Experimental/Gameplay2D/ActorComponent.hpp>
#include <Pomdog.Experimental/Gameplay2D/TextRenderable.hpp>
#include <Pomdog.Experimental/Gameplay2D/Transform.hpp>
#include <gtest/iutest_switch.hpp>
#include <cstdint>
#include <memory>

using namespace Pomdog;

TEST(Entity, AddComponent)
{
    EntityContext context;
    Entity entity{&context, context.Create({
        AddComponent<Transform>(),
        AddComponent<ActorComponent>()
    })};

    EXPECT_TRUE(entity.HasComponent<Transform>());
    EXPECT_TRUE(entity.GetComponent<Transform>());

    EXPECT_TRUE(entity.HasComponent<ActorComponent>());
    EXPECT_TRUE(entity.GetComponent<ActorComponent>());

    entity.GetComponent<Transform>()->SetPosition({3.0f, 4.0f, 5.0f});

    EXPECT_EQ(Vector3(3.0f, 4.0f, 5.0f), entity.GetComponent<Transform>()->GetPosition());
}

TEST(Entity, AddComponent_WithInheritance)
{
    EntityContext context;
    Entity entity{&context, context.Create({
        AddComponent<Transform>(),
        AddComponent<TextRenderable>()
    })};

    EXPECT_TRUE(entity.HasComponent<TextRenderable>());
    EXPECT_TRUE(entity.HasComponent<GraphicsComponent>());

    ASSERT_TRUE(entity.GetComponent<TextRenderable>());
    ASSERT_TRUE(entity.GetComponent<GraphicsComponent>());

    auto text = entity.GetComponent<TextRenderable>();
    text->SetDrawOrder(42);
    EXPECT_EQ(42, entity.GetComponent<TextRenderable>()->GetDrawOrder());
    EXPECT_EQ(42, entity.GetComponent<GraphicsComponent>()->GetDrawOrder());

    auto graphicsComponent = entity.GetComponent<GraphicsComponent>();
    graphicsComponent->SetDrawOrder(73);
    EXPECT_EQ(73, entity.GetComponent<TextRenderable>()->GetDrawOrder());
    EXPECT_EQ(73, entity.GetComponent<GraphicsComponent>()->GetDrawOrder());

    EXPECT_TRUE(std::dynamic_pointer_cast<TextRenderable>(graphicsComponent));
    EXPECT_EQ(text, std::dynamic_pointer_cast<TextRenderable>(graphicsComponent));
    EXPECT_EQ(73, std::dynamic_pointer_cast<TextRenderable>(graphicsComponent)->GetDrawOrder());
}

TEST(Entity, Component_Const)
{
    EntityContext context;
    auto entity = std::make_shared<Entity>(&context, context.Create({
        AddComponent<Transform>()
    }));

    EXPECT_TRUE(entity->HasComponent<Transform>());
    ASSERT_NE(nullptr, entity->GetComponent<Transform>());

    entity->GetComponent<Transform>()->SetPositionX(42.0f);
    EXPECT_EQ(42.0f, entity->GetComponent<Transform>()->GetPosition().X);

    {
        std::shared_ptr<Entity const> entityConstRef = entity;
        EXPECT_TRUE(entityConstRef->HasComponent<Transform>());
        ASSERT_NE(nullptr, entityConstRef->GetComponent<Transform>());
        EXPECT_EQ(42.0f, entityConstRef->GetComponent<Transform>()->GetPosition().X);
    }
}

namespace {

struct Behavior final : public Pomdog::Component {
    void Do(Pomdog::Entity & self)
    {
        EXPECT_TRUE(self);
        EXPECT_TRUE(self.HasComponent<Behavior>());
        self.DestroyImmediate();
        EXPECT_FALSE(self);
        //EXPECT_FALSE(self.HasComponent<Behavior>());
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

TEST(Entity, DestroyImmediate)
{
    {
        EntityContext context;
        Entity entity{&context, context.Create({})};

        EXPECT_TRUE(entity);
        EXPECT_EQ(1, context.GetCount());
        entity.DestroyImmediate();
        EXPECT_FALSE(entity);
        EXPECT_EQ(0, context.GetCount());
    }
    {
        EntityContext context;
        Entity entity{&context, context.Create({})};

        EXPECT_TRUE(entity);
        EXPECT_EQ(1, context.GetCount());
        context.DestroyImmediate(entity.GetID());
        EXPECT_FALSE(entity);
        EXPECT_EQ(0, context.GetCount());
    }
    {
        EntityContext context;
        Entity entity1{&context, context.Create({})};
        Entity entity2{&context, context.Create({})};
        Entity entity3{&context, context.Create({})};

        EXPECT_TRUE(entity1);
        EXPECT_TRUE(entity2);
        EXPECT_TRUE(entity3);
        EXPECT_EQ(3, context.GetCount());
        entity1.DestroyImmediate();
        EXPECT_FALSE(entity1);
        EXPECT_TRUE(entity2);
        EXPECT_TRUE(entity3);
        EXPECT_EQ(2, context.GetCount());
        entity3.DestroyImmediate();
        EXPECT_FALSE(entity1);
        EXPECT_TRUE(entity2);
        EXPECT_FALSE(entity3);
        EXPECT_EQ(1, context.GetCount());
        entity2.DestroyImmediate();
        EXPECT_FALSE(entity1);
        EXPECT_FALSE(entity2);
        EXPECT_FALSE(entity3);
        EXPECT_EQ(0, context.GetCount());
    }

    {
        EntityContext context;
        Entity entity{&context, context.Create({
            AddComponent<Behavior>()
        })};
        auto behavior = entity.GetComponent<Behavior>();
        ASSERT_NE(nullptr, behavior);
        std::weak_ptr<int> weak = behavior->ptr;

        EXPECT_TRUE(entity);
        EXPECT_TRUE(entity.HasComponent<Behavior>());
        EXPECT_EQ(behavior, entity.GetComponent<Behavior>());
        EXPECT_FALSE(weak.expired());
        EXPECT_EQ(1, context.GetCount());
        behavior->Do(entity);
        EXPECT_FALSE(entity);
        //EXPECT_FALSE(entity.HasComponent<Behavior>());
        //EXPECT_FALSE(entity.GetComponent<Behavior>());
        EXPECT_TRUE(behavior);
        EXPECT_FALSE(weak.expired());
        behavior.reset();
        EXPECT_TRUE(weak.expired());
        EXPECT_EQ(0, context.GetCount());
    }
}

TEST(Entity, Destroy)
{
    {
        EntityContext context;
        Entity entity{&context, context.Create({})};

        EXPECT_TRUE(entity);
        EXPECT_EQ(1, context.GetCount());
        entity.Destroy();
        EXPECT_FALSE(entity);
        EXPECT_EQ(0, context.GetCount());
        context.Refresh();
        EXPECT_FALSE(entity);
        EXPECT_EQ(0, context.GetCount());
    }
    {
        EntityContext context;
        Entity entity{&context, context.Create({})};

        EXPECT_TRUE(entity);
        EXPECT_EQ(1, context.GetCount());
        context.Destroy(entity.GetID());
        EXPECT_FALSE(entity);
        EXPECT_EQ(0, context.GetCount());
        context.Refresh();
        EXPECT_FALSE(entity);
        EXPECT_EQ(0, context.GetCount());
    }
    {
        EntityContext context;
        Entity entity1{&context, context.Create({})};
        Entity entity2{&context, context.Create({})};
        Entity entity3{&context, context.Create({})};

        EXPECT_TRUE(entity1);
        EXPECT_TRUE(entity2);
        EXPECT_TRUE(entity3);
        EXPECT_EQ(3, context.GetCount());
        entity1.Destroy();
        EXPECT_FALSE(entity1);
        EXPECT_TRUE(entity2);
        EXPECT_TRUE(entity3);
        EXPECT_EQ(2, context.GetCount());
        entity3.Destroy();
        EXPECT_FALSE(entity1);
        EXPECT_TRUE(entity2);
        EXPECT_FALSE(entity3);
        EXPECT_EQ(1, context.GetCount());
        entity2.Destroy();
        EXPECT_FALSE(entity1);
        EXPECT_FALSE(entity2);
        EXPECT_FALSE(entity3);
        EXPECT_EQ(0, context.GetCount());
        context.Refresh();
        EXPECT_FALSE(entity1);
        EXPECT_FALSE(entity2);
        EXPECT_FALSE(entity3);
        EXPECT_EQ(0, context.GetCount());
    }

    {
        EntityContext context;
        Entity entity{&context, context.Create({
            AddComponent<Behavior>()
        })};
        auto behavior = entity.GetComponent<Behavior>();
        ASSERT_NE(nullptr, behavior);
        std::weak_ptr<int> weak = behavior->ptr;

        EXPECT_TRUE(entity);
        EXPECT_TRUE(entity.HasComponent<Behavior>());
        EXPECT_EQ(behavior, entity.GetComponent<Behavior>());
        EXPECT_FALSE(weak.expired());
        EXPECT_EQ(1, context.GetCount());
        behavior->Do(entity);
        EXPECT_FALSE(entity);
        //EXPECT_FALSE(entity.HasComponent<Behavior>());
        //EXPECT_EQ(nullptr, entity.GetComponent<Behavior>());
        EXPECT_FALSE(weak.expired());
        EXPECT_EQ(0, context.GetCount());
        context.Refresh();
        EXPECT_FALSE(entity);
        //EXPECT_FALSE(entity.HasComponent<Behavior>());
        //EXPECT_FALSE(entity.GetComponent<Behavior>());
        EXPECT_FALSE(weak.expired());
        EXPECT_TRUE(behavior);
        behavior.reset();
        EXPECT_TRUE(weak.expired());
        EXPECT_EQ(0, context.GetCount());
    }
}

TEST(Entity, Clear)
{
    {
        EntityContext context;
        Entity entity1{&context, context.Create({})};
        Entity entity2{&context, context.Create({})};
        Entity entity3{&context, context.Create({})};

        EXPECT_TRUE(entity1);
        EXPECT_TRUE(entity2);
        EXPECT_TRUE(entity3);
        EXPECT_EQ(3, context.GetCount());
        context.Clear();
        EXPECT_FALSE(entity1);
        EXPECT_FALSE(entity2);
        EXPECT_FALSE(entity3);
        EXPECT_EQ(0, context.GetCount());
    }
    {
        EntityContext context;
        Entity entity1{&context, context.Create({
            AddComponent<Transform>(),
            AddComponent<ActorComponent>()
        })};
        Entity entity2{&context, context.Create({
            AddComponent<Transform>()
        })};
        Entity entity3{&context, context.Create({
            AddComponent<ActorComponent>()
        })};

        EXPECT_TRUE(entity1);
        EXPECT_TRUE(entity2);
        EXPECT_TRUE(entity3);
        EXPECT_NE(entity1.GetID(), entity2.GetID());
        EXPECT_NE(entity2.GetID(), entity3.GetID());
        EXPECT_EQ(3, context.GetCount());
        context.Clear();
        EXPECT_FALSE(entity1);
        EXPECT_FALSE(entity2);
        EXPECT_FALSE(entity3);
        EXPECT_EQ(0, context.GetCount());
    }
    {
        EntityContext context;
        Entity entity{&context, context.Create({})};
        auto oldId = entity.GetID();

        EXPECT_TRUE(entity);
        EXPECT_EQ(1, context.GetCount());
        context.Clear();
        EXPECT_FALSE(entity);
        EXPECT_EQ(0, context.GetCount());

        entity = Entity{&context, context.Create({})};
        auto newId = entity.GetID();
        EXPECT_NE(oldId, newId);
        EXPECT_NE(oldId.SequenceNumber(), newId.SequenceNumber());
        EXPECT_EQ(oldId.Index(), newId.Index());
    }

    {
        EntityContext context;
        Entity entity{&context, context.Create({
            AddComponent<Behavior>()
        })};
        auto behavior = entity.GetComponent<Behavior>();
        ASSERT_NE(nullptr, behavior);
        std::weak_ptr<int> weak = behavior->ptr;

        EXPECT_TRUE(entity);
        EXPECT_TRUE(entity.HasComponent<Behavior>());
        EXPECT_EQ(behavior, entity.GetComponent<Behavior>());
        EXPECT_FALSE(weak.expired());
        EXPECT_EQ(1, context.GetCount());
        context.Clear();
        EXPECT_FALSE(entity);
        EXPECT_TRUE(behavior);
        EXPECT_FALSE(weak.expired());
        behavior.reset();
        EXPECT_TRUE(weak.expired());
        EXPECT_EQ(0, context.GetCount());
    }
    {
        EntityContext context;
        Entity entity1{&context, context.Create({
            AddComponent<Behavior>()
        })};
        Entity entity2{&context, context.Create({})};
        auto behavior = entity1.GetComponent<Behavior>();
        ASSERT_NE(nullptr, behavior);
        std::weak_ptr<int> weak = behavior->ptr;

        auto oldId1 = entity1.GetID();
        auto oldId2 = entity2.GetID();

        EXPECT_TRUE(entity1);
        EXPECT_TRUE(entity2);
        EXPECT_EQ(2, context.GetCount());
        context.Clear();
        EXPECT_FALSE(entity1);
        EXPECT_FALSE(entity2);
        EXPECT_EQ(0, context.GetCount());

        entity1 = Entity{&context, context.Create({})};
        entity2 = Entity{&context, context.Create({})};

        EXPECT_NE(oldId1, oldId2);
        EXPECT_NE(oldId1, entity1.GetID());
        EXPECT_NE(oldId2, entity2.GetID());
        EXPECT_NE(oldId1.SequenceNumber(), entity1.GetID().SequenceNumber());
        EXPECT_NE(oldId2.SequenceNumber(), entity2.GetID().SequenceNumber());
        EXPECT_EQ(oldId1.Index(), entity1.GetID().Index());
        EXPECT_EQ(oldId2.Index(), entity2.GetID().Index());
    }
}

TEST(Entity, EntityID)
{
    EntityContext context;
    Entity entity{&context, context.Create({})};

    auto id = entity.GetID();
    EXPECT_EQ(id, entity.GetID());

    Entity entity2{&context, context.Create({})};

    EXPECT_NE(entity.GetID(), entity2.GetID());
    EXPECT_EQ(0, entity.GetID().Index());
    EXPECT_EQ(1, entity2.GetID().Index());
    EXPECT_EQ(1, entity.GetID().SequenceNumber());
    EXPECT_EQ(1, entity2.GetID().SequenceNumber());
}

TEST(Entity, EntityID_Sequence)
{
    EntityContext context;
    {
        Entity entity{&context, context.Create({})};
        EXPECT_EQ(0U, entity.GetID().Index());
        EXPECT_NE(0U, entity.GetID().SequenceNumber());
        EXPECT_EQ(1U, entity.GetID().SequenceNumber());
    }
    {
        Entity entity{&context, context.Create({})};
        EXPECT_EQ(1U, entity.GetID().Index());
        EXPECT_NE(0U, entity.GetID().SequenceNumber());
        EXPECT_EQ(1U, entity.GetID().SequenceNumber());
        entity.DestroyImmediate();
    }
    {
        Entity entity{&context, context.Create({})};
        EXPECT_EQ(1U, entity.GetID().Index());
        EXPECT_NE(0U, entity.GetID().SequenceNumber());
        EXPECT_EQ(2U, entity.GetID().SequenceNumber());
        entity.DestroyImmediate();
    }
    {
        Entity entity{&context, context.Create({})};
        EXPECT_EQ(1U, entity.GetID().Index());
        EXPECT_NE(0U, entity.GetID().SequenceNumber());
        EXPECT_EQ(3U, entity.GetID().SequenceNumber());
        entity.DestroyImmediate();
    }
    {
        Entity entity1{&context, context.Create({})};
        EXPECT_EQ(1U, entity1.GetID().Index());
        EXPECT_NE(0U, entity1.GetID().SequenceNumber());
        EXPECT_EQ(4U, entity1.GetID().SequenceNumber());

        Entity entity2{&context, context.Create({})};
        EXPECT_EQ(2U, entity2.GetID().Index());
        EXPECT_NE(0U, entity2.GetID().SequenceNumber());
        EXPECT_EQ(1U, entity2.GetID().SequenceNumber());

        Entity entity3{&context, context.Create({})};
        EXPECT_EQ(3U, entity3.GetID().Index());
        EXPECT_NE(0U, entity3.GetID().SequenceNumber());
        EXPECT_EQ(1U, entity3.GetID().SequenceNumber());

        entity1.DestroyImmediate();
        entity2.DestroyImmediate();
        entity3.DestroyImmediate();
    }
}

TEST(Entity, Cast_Bool)
{
    EntityContext context;
    {
        Entity entity{&context, context.Create({})};
        EXPECT_TRUE(entity);
        entity.DestroyImmediate();
        EXPECT_FALSE(entity);
    }
    {
        Entity entity1{&context, context.Create({})};
        Entity entity2{&context, context.Create({})};
        EXPECT_TRUE(entity1);
        EXPECT_TRUE(entity2);
        entity1.DestroyImmediate();
        EXPECT_FALSE(entity1);
        EXPECT_TRUE(entity2);
        entity2.DestroyImmediate();
        EXPECT_FALSE(entity1);
        EXPECT_FALSE(entity2);
    }
    {
        Entity entity{&context, context.Create({})};
        auto copiedObject = entity;
        EXPECT_TRUE(entity);
        EXPECT_TRUE(copiedObject);
        entity.DestroyImmediate();
        EXPECT_FALSE(entity);
        EXPECT_FALSE(copiedObject);
    }
    {
        Entity entity{&context, context.Create({})};
        auto copiedObject = entity;
        EXPECT_TRUE(entity);
        EXPECT_TRUE(copiedObject);
        copiedObject.DestroyImmediate();
        EXPECT_FALSE(entity);
        EXPECT_FALSE(copiedObject);
    }
}

TEST(Entity, EqualOperator)
{
    EntityContext context;
    Entity entity1{&context, context.Create({})};
    Entity entity2{&context, context.Create({})};
    EXPECT_NE(entity1, entity2);
    EXPECT_EQ(entity1, entity1);
    EXPECT_EQ(entity2, entity2);

    Entity entity3 = entity1;
    EXPECT_EQ(entity3, entity1);
    EXPECT_NE(entity3, entity2);

    entity3.DestroyImmediate();
    EXPECT_NE(entity3, entity1);
    EXPECT_NE(entity3, entity2);

    entity1.DestroyImmediate();
    EXPECT_EQ(entity1, entity3);
    EXPECT_NE(entity1, entity2);

    entity2.DestroyImmediate();
    EXPECT_EQ(entity2, entity1);
    EXPECT_EQ(entity2, entity3);
}

TEST(Entity, EntityID_Unique)
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

                for (auto & object: objects) {
                    ASSERT_TRUE(object);
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

    ASSERT_FALSE(uniqueIdents.empty());

    std::sort(std::begin(uniqueIdents), std::end(uniqueIdents));
    auto iter = std::adjacent_find(std::begin(uniqueIdents), std::end(uniqueIdents));
#ifdef _MSC_VER
    EXPECT_TRUE(std::end(uniqueIdents) == iter);
#else
    EXPECT_EQ(std::end(uniqueIdents), iter);
#endif
}
