// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Signals/Connection.hpp>
#include <Pomdog/Signals/Event.hpp>
#include <Pomdog/Signals/EventQueue.hpp>
#include <gtest/iutest_switch.hpp>
#include <utility>

using Pomdog::Event;
using Pomdog::EventQueue;
using Pomdog::Connection;

struct EventQueueTest: public ::testing::Test
{
    std::vector<int> integers;
    std::vector<std::string> names;
    std::function<void(Event const&)> slot;

    void SetUp() override
    {
        integers.clear();
        names.clear();

        slot = [&](Event const& event) {
            ASSERT_TRUE(event.Is<int>());
            integers.push_back(*event.As<int>());
        };
    }
};


TEST_F(EventQueueTest, InvokeInt)
{
    EventQueue eventQueue;
    auto connection = eventQueue.Connect(slot);

    eventQueue.Enqueue<int>(42);
    eventQueue.Enqueue<int>(43);
    eventQueue.Enqueue<int>(44);
    ASSERT_TRUE(integers.empty());

    eventQueue.Emit();
    ASSERT_EQ(3, integers.size());
    EXPECT_EQ(42, integers[0]);
    EXPECT_EQ(43, integers[1]);
    EXPECT_EQ(44, integers[2]);
}

TEST_F(EventQueueTest, InvokePODStruct)
{
    struct User
    {
        std::string name;
        int id;
    };

    EventQueue eventQueue;
    eventQueue.Connect([&](Event const& event){
        ASSERT_TRUE(event.Is<User>());
        auto user = event.As<User>();
        names.push_back(user->name);
        integers.push_back(user->id);
    });

    eventQueue.Enqueue<User>("Donald", 42);
    eventQueue.Enqueue<User>("Goofy", 43);
    ASSERT_TRUE(names.empty());
    ASSERT_TRUE(integers.empty());

    eventQueue.Emit();
    ASSERT_EQ(2, names.size());
    EXPECT_EQ("Donald", names[0]);
    EXPECT_EQ("Goofy", names[1]);
    ASSERT_EQ(2, integers.size());
    EXPECT_EQ(42, integers[0]);
    EXPECT_EQ(43, integers[1]);
}

TEST_F(EventQueueTest, Disconnect)
{
    EventQueue eventQueue;
    auto connection = eventQueue.Connect(slot);

    eventQueue.Enqueue<int>(42);
    ASSERT_TRUE(integers.empty());

    connection.Disconnect();
    eventQueue.Emit();
    ASSERT_TRUE(integers.empty());
}

TEST_F(EventQueueTest, Conenct)
{
    EventQueue eventQueue;
    eventQueue.Connect(slot);
    eventQueue.Connect(slot);
    eventQueue.Connect(slot);

    eventQueue.Enqueue<int>(42);
    ASSERT_TRUE(integers.empty());

    eventQueue.Emit();
    ASSERT_EQ(3, integers.size());
    EXPECT_EQ(42, integers[0]);
    EXPECT_EQ(42, integers[1]);
    EXPECT_EQ(42, integers[2]);
}

TEST_F(EventQueueTest, RecursiveConnection)
{
    EventQueue eventQueue;
    auto connection = eventQueue.Connect([&](Event const&) {
        eventQueue.Connect(slot);
    });

    eventQueue.Enqueue<int>(42);
    ASSERT_TRUE(integers.empty());
    eventQueue.Emit();
    ASSERT_TRUE(integers.empty());

    eventQueue.Enqueue<int>(43);
    ASSERT_TRUE(integers.empty());

    eventQueue.Emit();
    ASSERT_EQ(1, integers.size());
    EXPECT_EQ(43, integers[0]);
}

TEST_F(EventQueueTest, CallingDisconnect)
{
    EventQueue eventQueue;
    Connection connection;
    connection = eventQueue.Connect([&](Event const& event){
        slot(event);
        connection.Disconnect();
    });

    eventQueue.Enqueue<int>(42);
    ASSERT_TRUE(integers.empty());
    eventQueue.Emit();
    ASSERT_EQ(1, integers.size());
    EXPECT_EQ(42, integers[0]);

    integers.clear();
    eventQueue.Enqueue<int>(43);
    ASSERT_TRUE(integers.empty());
    eventQueue.Emit();
    ASSERT_TRUE(integers.empty());
}

TEST_F(EventQueueTest, ArgumentPerfectForwarding)
{
    EventQueue eventQueue;
    auto connection = eventQueue.Connect([&](Event const& event){
        ASSERT_TRUE(event.Is<std::shared_ptr<int>>());
    });

    {
        auto pointer = std::make_shared<int>(42);
        ASSERT_TRUE(pointer);
        eventQueue.Enqueue<std::shared_ptr<int>>(pointer);
        EXPECT_TRUE(pointer);
    }
    {
        auto pointer = std::make_shared<int>(42);
        ASSERT_TRUE(pointer);
        eventQueue.Enqueue<std::shared_ptr<int>>(std::move(pointer));
        EXPECT_FALSE(pointer);
    }
    {
        auto const pointer = std::make_shared<int>(42);
        ASSERT_TRUE(pointer);
        eventQueue.Enqueue<std::shared_ptr<int>>(pointer);
        EXPECT_TRUE(pointer);
    }
}
