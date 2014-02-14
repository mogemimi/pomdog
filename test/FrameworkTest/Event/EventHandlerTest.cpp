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
#include <Pomdog/Event/EventHandler.hpp>

using Pomdog::Event;
using Pomdog::EventHandler;
using Pomdog::EventConnection;

struct EventHandlerTest: public ::testing::Test
{
	EventHandler handler;
	std::vector<int> integers;
	std::function<void(Event const&)> slot;

	void SetUp() override
	{
		integers.clear();
		EventHandler eventHandlerNew;
		handler = std::move(eventHandlerNew);
		
		slot = [&](Event const& event) {
			ASSERT_TRUE(event.Is<int>());
			integers.push_back(*event.As<int>());
		};
	}
};

TEST_F(EventHandlerTest, InvokeInt)
{
	auto connection = handler.Connect(slot);

	handler.Invoke<int>(42);
	handler.Invoke<int>(43);
	handler.Invoke<int>(44);

	ASSERT_EQ(3, integers.size());
	EXPECT_EQ(42, integers[0]);
	EXPECT_EQ(43, integers[1]);
	EXPECT_EQ(44, integers[2]);
}

TEST_F(EventHandlerTest, Disconnect)
{
	auto connection = handler.Connect(slot);

	handler.Invoke<int>(42);
	handler.Invoke<int>(43);
	connection.Disconnect();
	handler.Invoke<int>(44);

	ASSERT_EQ(2, integers.size());
	EXPECT_EQ(42, integers[0]);
	EXPECT_EQ(43, integers[1]);
}

TEST_F(EventHandlerTest, MoveConstructor)
{
	auto connection = handler.Connect(slot);
	Pomdog::EventHandler handlerNew = std::move(handler);
	
	handlerNew.Invoke<int>(42);
	connection.Disconnect();
	handlerNew.Invoke<int>(43);

	ASSERT_EQ(1, integers.size());
	EXPECT_EQ(42, integers[0]);
}

//TEST_F(EventHandlerTest, MoveConstructor_Reconnect)
//{
//	Pomdog::EventHandler handlerOld;
//	Pomdog::EventHandler handlerNew = std::move(handlerOld);
//	auto connection = handlerOld.Connect(slot);
//	
//	handlerNew.Invoke<int>(42);
//	connection.Disconnect();
//	handlerNew.Invoke<int>(43);
//
//	ASSERT_EQ(1, integers.size());
//	EXPECT_EQ(42, integers[0]);
//}

TEST_F(EventHandlerTest, MoveAssignment)
{
	auto connection = handler.Connect(slot);
	Pomdog::EventHandler handlerNew;
	handlerNew.Connect([&](Event const& event) {
		ASSERT_TRUE(event.Is<int>());
		integers.push_back(*event.As<int>() + 900);
	});
	
	handlerNew.Invoke<int>(42);
	handler.Invoke<int>(43);
	handlerNew = std::move(handler);
	handlerNew.Invoke<int>(44);
	connection.Disconnect();
	handlerNew.Invoke<int>(45);

	ASSERT_EQ(3, integers.size());
	EXPECT_EQ(942, integers[0]);
	EXPECT_EQ(43, integers[1]);
	EXPECT_EQ(44, integers[2]);
}

TEST_F(EventHandlerTest, ConnectSomeSlots)
{
	handler.Connect(slot);
	handler.Connect(slot);
	handler.Connect(slot);
	
	handler.Invoke<int>(42);

	ASSERT_EQ(3, integers.size());
	EXPECT_EQ(42, integers[0]);
	EXPECT_EQ(42, integers[1]);
	EXPECT_EQ(42, integers[2]);
}

TEST_F(EventHandlerTest, DisconnectSlots)
{
	auto connection = handler.Connect(slot);
	handler.Connect(slot);
	handler.Connect(slot);
	
	handler.Invoke<int>(42);
	connection.Disconnect();
	handler.Invoke<int>(72);

	ASSERT_EQ(5, integers.size());
	EXPECT_EQ(42, integers[0]);
	EXPECT_EQ(42, integers[1]);
	EXPECT_EQ(42, integers[2]);
	EXPECT_EQ(72, integers[3]);
	EXPECT_EQ(72, integers[4]);
}
