//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <gtest/iutest_switch.hpp>
#include <utility>
#include <Pomdog/Event/EventConnection.hpp>
#include <Pomdog/Event/Event.hpp>
#include <Pomdog/Event/EventQueue.hpp>

using Pomdog::Event;
using Pomdog::EventQueue;
using Pomdog::EventConnection;

struct EventQueueTest: public ::testing::Test
{
	std::vector<int> integers;
	std::function<void(Event const&)> slot;

	void SetUp() override
	{
		integers.clear();
		
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
	
	eventQueue.Tick();
	ASSERT_EQ(3, integers.size());
	EXPECT_EQ(42, integers[0]);
	EXPECT_EQ(43, integers[1]);
	EXPECT_EQ(44, integers[2]);
}

TEST_F(EventQueueTest, Disconnect)
{
	EventQueue eventQueue;
	auto connection = eventQueue.Connect(slot);

	eventQueue.Enqueue<int>(42);
	ASSERT_TRUE(integers.empty());
	
	connection.Disconnect();
	eventQueue.Tick();
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
	
	eventQueue.Tick();
	ASSERT_EQ(3, integers.size());
	EXPECT_EQ(42, integers[0]);
	EXPECT_EQ(42, integers[1]);
	EXPECT_EQ(42, integers[2]);
}

TEST_F(EventQueueTest, RecursiveConnection)
{
	EventQueue eventQueue;
	auto connection = eventQueue.Connect([&](Event const& event){
		eventQueue.Connect(slot);
	});
	
	eventQueue.Enqueue<int>(42);
	ASSERT_TRUE(integers.empty());
	eventQueue.Tick();
	ASSERT_TRUE(integers.empty());
	
	eventQueue.Enqueue<int>(43);
	ASSERT_TRUE(integers.empty());
	
	eventQueue.Tick();
	ASSERT_EQ(1, integers.size());
	EXPECT_EQ(43, integers[0]);
}

TEST_F(EventQueueTest, CallingDisconnect)
{
	EventQueue eventQueue;
	EventConnection connection;
	connection = eventQueue.Connect([&](Event const& event){
		slot(event);
		connection.Disconnect();
	});
	
	eventQueue.Enqueue<int>(42);
	ASSERT_TRUE(integers.empty());
	eventQueue.Tick();
	ASSERT_EQ(1, integers.size());
	EXPECT_EQ(42, integers[0]);
	
	integers.clear();
	eventQueue.Enqueue<int>(43);
	ASSERT_TRUE(integers.empty());
	eventQueue.Tick();
	ASSERT_TRUE(integers.empty());
}
