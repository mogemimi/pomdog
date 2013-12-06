//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <iutest/gtest/iutest_switch.hpp>
#include <Pomdog/Logging/Log.hpp>
#include <Pomdog/Event/Event.hpp>
#include <Pomdog/Event/ScopedConnection.hpp>
#include <vector>

using Pomdog::Log;
using Pomdog::LogChannel;
using Pomdog::LogEntry;
using Pomdog::LoggingLevel;
using Pomdog::LogStream;
using Pomdog::ScopedConnection;

TEST(LoggingTest, LogMessageWithDefaultChannel)
{
	std::string message;
	std::string source;
	ScopedConnection connection(Log::Connect([&](LogEntry const& entry) {
		message = entry.message;
		source = entry.source;
	}));
	
	Log::LogMessage("Hello");
	EXPECT_EQ("Hello", message);
	EXPECT_TRUE(source.empty());
	
	Log::LogMessage("Hi");
	EXPECT_EQ("Hi", message);
	EXPECT_TRUE(source.empty());
}

TEST(LoggingTest, StreamWithDefaultChannel)
{
	std::string message;
	std::string source;
	ScopedConnection connection(Log::Connect([&](LogEntry const& entry){
		message = entry.message;
		source = entry.source;
	}));

	Log::Stream() << "Hello, " << "World.";
	EXPECT_EQ("Hello, World.", message);
	EXPECT_TRUE(source.empty());
}

TEST(LoggingTest, AddAndRemoveChannel)
{
	ASSERT_EQ(false, Log::ExistChannel("Test"));
	ASSERT_EQ(false, Log::ExistChannel("Test2"));
	
	Log::AddChannel("Test", LoggingLevel::Internal);
	EXPECT_EQ(true, Log::ExistChannel("Test"));
	EXPECT_EQ(false, Log::ExistChannel("Test2"));
	
	Log::AddChannel("Test2", LoggingLevel::Internal);
	EXPECT_EQ(true, Log::ExistChannel("Test"));
	EXPECT_EQ(true, Log::ExistChannel("Test2"));
	
	Log::RemoveChannel("Test");
	EXPECT_EQ(false, Log::ExistChannel("Test"));
	EXPECT_EQ(true, Log::ExistChannel("Test2"));
	
	Log::RemoveChannel("Test2");
	EXPECT_EQ(false, Log::ExistChannel("Test"));
	EXPECT_EQ(false, Log::ExistChannel("Test2"));
	
	Log::AddChannel("Test2");
	EXPECT_EQ(false, Log::ExistChannel("Test"));
	EXPECT_EQ(true, Log::ExistChannel("Test2"));
	
	Log::RemoveChannel("Test2");
	EXPECT_EQ(false, Log::ExistChannel("Test"));
	EXPECT_EQ(false, Log::ExistChannel("Test2"));
	
	Log::AddChannel("Test", LoggingLevel::Internal);
	EXPECT_EQ(true, Log::ExistChannel("Test"));
	EXPECT_EQ(false, Log::ExistChannel("Test2"));
	
	Log::RemoveChannel("Test2");
	EXPECT_EQ(true, Log::ExistChannel("Test"));
	EXPECT_EQ(false, Log::ExistChannel("Test2"));
	
	Log::RemoveChannel("Test");
	EXPECT_EQ(false, Log::ExistChannel("Test"));
	EXPECT_EQ(false, Log::ExistChannel("Test2"));
}

TEST(LoggingTest, SetDefaultVerbosityLevels)
{
	auto verbosity = Log::GetVerbosity();

	Log::SetVerbosity(LoggingLevel::Internal);
	EXPECT_EQ(LoggingLevel::Internal, Log::GetVerbosity());
	
	Log::SetVerbosity(LoggingLevel::Verbose);
	EXPECT_EQ(LoggingLevel::Verbose, Log::GetVerbosity());
	
	Log::SetVerbosity(LoggingLevel::Brief);
	EXPECT_EQ(LoggingLevel::Brief, Log::GetVerbosity());
	
	Log::SetVerbosity(LoggingLevel::Critical);
	EXPECT_EQ(LoggingLevel::Critical, Log::GetVerbosity());
	
	Log::SetVerbosity(verbosity);
}

TEST(LoggingTest, SetVerbosityLevels)
{
	ASSERT_EQ(false, Log::ExistChannel("Test"));

	Log::AddChannel("Test", LoggingLevel::Internal);
	EXPECT_EQ(LoggingLevel::Internal, Log::GetVerbosity("Test"));
	
	Log::RemoveChannel("Test");
	Log::AddChannel("Test", LoggingLevel::Verbose);
	EXPECT_EQ(LoggingLevel::Verbose, Log::GetVerbosity("Test"));
	
	Log::RemoveChannel("Test");
	Log::AddChannel("Test", LoggingLevel::Brief);
	EXPECT_EQ(LoggingLevel::Brief, Log::GetVerbosity("Test"));
	
	Log::RemoveChannel("Test");
	Log::AddChannel("Test", LoggingLevel::Critical);
	EXPECT_EQ(LoggingLevel::Critical, Log::GetVerbosity("Test"));
	
	Log::SetVerbosity("Test", LoggingLevel::Internal);
	EXPECT_EQ(LoggingLevel::Internal, Log::GetVerbosity("Test"));
	
	Log::SetVerbosity("Test", LoggingLevel::Verbose);
	EXPECT_EQ(LoggingLevel::Verbose, Log::GetVerbosity("Test"));
	
	Log::SetVerbosity("Test", LoggingLevel::Brief);
	EXPECT_EQ(LoggingLevel::Brief, Log::GetVerbosity("Test"));
	
	Log::SetVerbosity("Test", LoggingLevel::Critical);
	EXPECT_EQ(LoggingLevel::Critical, Log::GetVerbosity("Test"));
	
	Log::RemoveChannel("Test");
}

TEST(LoggingTest, SendToUserChannel)
{
	std::vector<std::string> messages, sources;
	auto handler = [&](LogEntry const& entry) {
		messages.push_back(entry.message);
		sources.push_back(entry.source);
	};
	
	ASSERT_TRUE(!Log::ExistChannel("Test"));
	ASSERT_TRUE(!Log::ExistChannel("NyanCat"));
	
	Log::AddChannel("Test", LoggingLevel::Internal);
	EXPECT_TRUE(Log::ExistChannel("Test"));
	
	ScopedConnection connection(Log::Connect(handler, "Test"));
	
	Log::LogMessage("Hello, world.", "Test");
	Log::LogMessage("Nyan Nyan Cat", "NyanCat");
	
	Log::RemoveChannel("Test");
	ASSERT_TRUE(!Log::ExistChannel("Test"));

	connection = Log::Connect(handler, "Test");
	Log::LogMessage("Create new channel and connect", "Test");
	
	Log::AddChannel("Test");
	connection = Log::Connect(handler, "Test");
	Log::LogMessage("Send to existent channel", "Test");
	
	ASSERT_EQ(3, messages.size());
	ASSERT_EQ(3, sources.size());
	
	EXPECT_EQ("Hello, world.", messages[0]);
	EXPECT_EQ("Test", sources[0]);
	EXPECT_EQ("Create new channel and connect", messages[1]);
	EXPECT_EQ("Test", sources[1]);
	EXPECT_EQ("Send to existent channel", messages[2]);
	EXPECT_EQ("Test", sources[2]);
	
	Log::RemoveChannel("Test");
}

TEST(LoggingTest, SendToUserChannels)
{
	Log::AddChannel("Test1", LoggingLevel::Internal);
	Log::AddChannel("Test2", LoggingLevel::Internal);
	
	std::vector<std::string> messages, sources;
	auto handler = [&](LogEntry const& entry){
		messages.push_back(entry.message);
		sources.push_back(entry.source);
	};
	
	ASSERT_TRUE(Log::ExistChannel("Test1"));
	ASSERT_TRUE(Log::ExistChannel("Test2"));
	ASSERT_TRUE(!Log::ExistChannel("Nyan Nyan Cat"));
	
	ScopedConnection connection0(Log::Connect(handler));
	ScopedConnection connection1(Log::Connect(handler, "Test1"));
	ScopedConnection connection2(Log::Connect(handler, "Test2"));
	
	Log::LogMessage("(A) Send to default channel");
	Log::LogMessage("(B) Send to channel Test1", "Test1");
	Log::LogMessage("(C) Send to channel Test2", "Test2");
	Log::LogMessage("(D) Send to nonexistent channel", "Nyan Nyan Cat");
	
	// Connect
	Log::AddChannel("Nyan Nyan Cat", LoggingLevel::Internal);
	ASSERT_TRUE(Log::ExistChannel("Nyan Nyan Cat"));
	
	ScopedConnection connection3(Log::Connect(handler, "Nyan Nyan Cat"));
	Log::LogMessage("(E) Send to new channel", "Nyan Nyan Cat");
	
	// Disconnect
	connection1.Disconnect();
	Log::LogMessage("(F) Send to channel Test1", "Test1");
	Log::LogMessage("(G) Send to channel Test2", "Test2");

	ASSERT_EQ(6, messages.size());
	ASSERT_EQ(6, sources.size());
	
	EXPECT_EQ(messages[0], "(A) Send to default channel");
	EXPECT_EQ(messages[1], "(B) Send to channel Test1");
	EXPECT_EQ(messages[2], "(C) Send to channel Test2");
	EXPECT_EQ(messages[3], "(D) Send to nonexistent channel");
	EXPECT_EQ(messages[4], "(E) Send to new channel");
	EXPECT_EQ(messages[5], "(G) Send to channel Test2");
	
	EXPECT_EQ(sources[0], "");
	EXPECT_EQ(sources[1], "Test1");
	EXPECT_EQ(sources[2], "Test2");
	EXPECT_EQ(sources[3], "");
	EXPECT_EQ(sources[4], "Nyan Nyan Cat");
	EXPECT_EQ(sources[5], "Test2");
	
	Log::RemoveChannel("Test1");
	Log::RemoveChannel("Test2");
	Log::RemoveChannel("Nyan Nyan Cat");
}

TEST(LoggingTest, CallToDisconnectInCallback)
{
	std::vector<std::string> messages;
	ScopedConnection connectionA, connectionB;
	
	connectionA = Log::Connect([&](LogEntry const& entry){
		messages.push_back(entry.message + " at A");

		if (entry.message == "Disconnect B") {
			connectionB.Disconnect();
		}
	});
	
	connectionB = Log::Connect([&](LogEntry const& entry){
		messages.push_back(entry.message + " at B");
	});
	
	Log::LogMessage("Hello");
	Log::LogMessage("Hi");
	Log::LogMessage("Disconnect B");
	Log::LogMessage("Chuck Norris");
	
	ASSERT_EQ(6, messages.size());
	EXPECT_EQ(messages[0], "Hello at A");
	EXPECT_EQ(messages[1], "Hello at B");
	EXPECT_EQ(messages[2], "Hi at A");
	EXPECT_EQ(messages[3], "Hi at B");
	EXPECT_EQ(messages[4], "Disconnect B at A");
	EXPECT_EQ(messages[5], "Chuck Norris at A");
}

TEST(LoggingTest, CallToDisconnectSelfInCallback)
{
	std::vector<std::string> messages;
	ScopedConnection connectionA;
	
	connectionA = Log::Connect([&](LogEntry const& entry){
		messages.push_back(entry.message + " at A");

		if (entry.message == "Disconnect") {
			connectionA.Disconnect();
		}
	});
	
	Log::LogMessage("Hello");
	Log::LogMessage("Hi");
	Log::LogMessage("Disconnect");
	Log::LogMessage("Chuck Norris");
	
	ASSERT_EQ(3, messages.size());
	EXPECT_EQ(messages[0], "Hello at A");
	EXPECT_EQ(messages[1], "Hi at A");
	EXPECT_EQ(messages[2], "Disconnect at A");
}
