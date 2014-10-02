//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <gtest/iutest_switch.hpp>
#include <utility>
#include <Pomdog/Event/Event.hpp>
#include <Pomdog/Event/EventConnection.hpp>
#include <Pomdog/Event/EventHandler.hpp>
#include <Pomdog/Event/ScopedConnection.hpp>

using Pomdog::Event;
using Pomdog::EventConnection;
using Pomdog::EventHandler;
using Pomdog::ScopedConnection;

TEST(ScopedConnection, ScopeGuard)
{
	EventHandler eventHandler;
	int count = 0;
	
	{
		ScopedConnection connection;
		connection = eventHandler.Connect([&](Event const& event){
			++count;
		});
		
		eventHandler.Invoke<std::string>("event");
		EXPECT_EQ(1, count);
		eventHandler.Invoke<std::string>("event");
		EXPECT_EQ(2, count);
	}
	
	eventHandler.Invoke<std::string>("event");
	EXPECT_EQ(2, count);
	eventHandler.Invoke<std::string>("event");
	EXPECT_EQ(2, count);
}

TEST(ScopedConnection, ExplicitDisconnect)
{
	EventHandler eventHandler;
	ScopedConnection connection;
	int count = 0;
	
	connection = eventHandler.Connect([&](Event const& event){
		++count;
	});
	
	eventHandler.Invoke<std::string>("event");
	EXPECT_EQ(1, count);
	eventHandler.Invoke<std::string>("event");
	EXPECT_EQ(2, count);
	
	connection.Disconnect();
	
	eventHandler.Invoke<std::string>("event");
	EXPECT_EQ(2, count);
	eventHandler.Invoke<std::string>("event");
	EXPECT_EQ(2, count);
}

TEST(ScopedConnection, MoveAssignment)
{
	EventHandler eventHandler;
	int count = 0;
	
	ScopedConnection connection1;
	{
		ScopedConnection connection2;
		
		connection2 = eventHandler.Connect([&](Event const& event){
			++count;
		});
		
		eventHandler.Invoke<std::string>("event");
		EXPECT_EQ(1, count);
		eventHandler.Invoke<std::string>("event");
		EXPECT_EQ(2, count);
		
		connection1 = std::move(connection2);
	}
	
	eventHandler.Invoke<std::string>("event");
	EXPECT_EQ(3, count);
	eventHandler.Invoke<std::string>("event");
	EXPECT_EQ(4, count);
	
	connection1.Disconnect();
	
	eventHandler.Invoke<std::string>("event");
	EXPECT_EQ(4, count);
	eventHandler.Invoke<std::string>("event");
	EXPECT_EQ(4, count);
}

TEST(ScopedConnection, CopyAssignmentEventConnection)
{
	EventHandler eventHandler;
	int count = 0;
	
	ScopedConnection connection1;
	{
		EventConnection connection2;
		
		connection2 = eventHandler.Connect([&](Event const& event){
			++count;
		});
		
		eventHandler.Invoke<std::string>("event");
		EXPECT_EQ(1, count);
		
		connection1 = connection2;
		
		eventHandler.Invoke<std::string>("event");
		EXPECT_EQ(2, count);
	}
	
	eventHandler.Invoke<std::string>("event");
	EXPECT_EQ(3, count);
	eventHandler.Invoke<std::string>("event");
	EXPECT_EQ(4, count);
	
	connection1.Disconnect();
	
	eventHandler.Invoke<std::string>("event");
	EXPECT_EQ(4, count);
	eventHandler.Invoke<std::string>("event");
	EXPECT_EQ(4, count);
}

TEST(ScopedConnection, MoveAssignmentEventConnection)
{
	EventHandler eventHandler;
	int count = 0;
	
	ScopedConnection connection1;
	{
		EventConnection connection2;
		
		connection2 = eventHandler.Connect([&](Event const& event){
			++count;
		});
		
		eventHandler.Invoke<std::string>("event");
		EXPECT_EQ(1, count);
		
		connection1 = std::move(connection2);
		
		eventHandler.Invoke<std::string>("event");
		EXPECT_EQ(2, count);
		
		connection2.Disconnect();
		
		eventHandler.Invoke<std::string>("event");
		EXPECT_EQ(3, count);
	}
	
	eventHandler.Invoke<std::string>("event");
	EXPECT_EQ(4, count);
	
	connection1.Disconnect();
	
	eventHandler.Invoke<std::string>("event");
	EXPECT_EQ(4, count);
}
