//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <iutest/gtest/iutest_switch.hpp>
#include <Pomdog/Event/Event.hpp>
#include <Pomdog/Event/EventConnection.hpp>
#include <Pomdog/Event/EventHandler.hpp>
#include <Pomdog/Event/ScopedConnection.hpp>
#include <utility>

using Pomdog::Event;
using Pomdog::EventCode;
using Pomdog::EventConnection;
using Pomdog::EventHandler;
using Pomdog::ScopedConnection;

TEST(ScopedConnection, ScopeGuard)
{
	EventHandler eventHandler;
	EventCode result{0};
	
	{
		ScopedConnection connection;
		connection = eventHandler.Connect([&](Event const& event){
			result = event.GetCode();
		});
		
		eventHandler.Trigger(std::make_shared<Event>(EventCode{123}));
	}
	EXPECT_EQ(EventCode{123}, result);
	
	eventHandler.Trigger(std::make_shared<Event>(EventCode{456}));
	EXPECT_EQ(EventCode{123}, result);
}

TEST(ScopedConnection, ExplicitDisconnect)
{
	EventHandler eventHandler;
	EventCode result{0};
	
	ScopedConnection connection;
	
	connection = eventHandler.Connect([&](Event const& event){
		result = event.GetCode();
	});
	
	eventHandler.Trigger(std::make_shared<Event>(EventCode{123}));
	EXPECT_EQ(EventCode{123}, result);
	
	connection.Disconnect();
	
	eventHandler.Trigger(std::make_shared<Event>(EventCode{456}));
	EXPECT_EQ(EventCode{123}, result);
}

TEST(ScopedConnection, MoveAssignment)
{
	EventHandler eventHandler;
	EventCode result{0};
	
	ScopedConnection connection1;
	{
		ScopedConnection connection2;
		
		connection2 = eventHandler.Connect([&](Event const& event){
			result = event.GetCode();
		});
		
		eventHandler.Trigger(std::make_shared<Event>(EventCode{789}));
		ASSERT_EQ(EventCode{789}, result);
		
		connection1 = std::move(connection2);
	}
	
	eventHandler.Trigger(std::make_shared<Event>(EventCode{123}));
	EXPECT_EQ(EventCode{123}, result);
	
	connection1.Disconnect();
	
	eventHandler.Trigger(std::make_shared<Event>(EventCode{456}));
	EXPECT_EQ(EventCode{123}, result);
}

TEST(ScopedConnection, CopyAssignmentEventConnection)
{
	EventHandler eventHandler;
	EventCode result{0};
	
	ScopedConnection connection1;
	{
		EventConnection connection2;
		
		connection2 = eventHandler.Connect([&](Event const& event){
			result = event.GetCode();
		});
		
		eventHandler.Trigger(std::make_shared<Event>(EventCode{789}));
		ASSERT_EQ(EventCode{789}, result);
		
		connection1 = connection2;
	}
	
	eventHandler.Trigger(std::make_shared<Event>(EventCode{123}));
	EXPECT_EQ(EventCode{123}, result);
	
	connection1.Disconnect();
	
	eventHandler.Trigger(std::make_shared<Event>(EventCode{456}));
	EXPECT_EQ(EventCode{123}, result);
}

TEST(ScopedConnection, MoveAssignmentEventConnection)
{
	EventHandler eventHandler;
	EventCode result{0};
	
	ScopedConnection connection1;
	{
		EventConnection connection2;
		
		connection2 = eventHandler.Connect([&](Event const& event){
			result = event.GetCode();
		});
		
		eventHandler.Trigger(std::make_shared<Event>(EventCode{789}));
		ASSERT_EQ(EventCode{789}, result);
		
		connection1 = std::move(connection2);
		connection2.Disconnect();
	}
	
	eventHandler.Trigger(std::make_shared<Event>(EventCode{123}));
	EXPECT_EQ(EventCode{123}, result);
	
	connection1.Disconnect();
	
	eventHandler.Trigger(std::make_shared<Event>(EventCode{456}));
	EXPECT_EQ(EventCode{123}, result);
}
