//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <iutest/gtest/iutest_switch.hpp>
#include <Pomdog/Logging/LogChannel.hpp>
#include <Pomdog/Logging/LogEntry.hpp>
#include <Pomdog/Logging/LogStream.hpp>
#include <Pomdog/Event/EventConnection.hpp>
#include <Pomdog/Event/ScopedConnection.hpp>
#include <ostream>

using Pomdog::LogChannel;
using Pomdog::LogEntry;
using Pomdog::LoggingLevel;
using Pomdog::LogStream;
using Pomdog::ScopedConnection;

TEST(LogStream, LogStreamFlush)
{
	LogChannel channel("test");
	LogStream stream(channel, LoggingLevel::Critical);
	
	std::string message;
	auto connection = channel.Connect([&message](LogEntry const& entry){
		message = entry.message;
	});
	
	message.clear();
	stream << "Chuck " << "Norris " << "Approval." << LogStream::Flush();
	EXPECT_EQ(message, "Chuck Norris Approval.");
	
	message.clear();
	stream << "Chuck ";
	stream << "Norris ";
	stream << "makes ";
	stream << "onions ";
	stream << "cry!";
	EXPECT_TRUE(message.empty());
	stream << LogStream::Flush();
	EXPECT_EQ(message, "Chuck Norris makes onions cry!");
}

TEST(LogStream, EnumerationToString)
{
	LogChannel channel("test");
	LogStream stream(channel, LoggingLevel::Critical);
	
	std::string message;
	auto connection = channel.Connect([&message](LogEntry const& entry){
		message = entry.message;
	});
	
	enum class Numbers: int
	{
		Zero = 0,
		One = 1,
		Two = 2,
		Three = 3,
		TheAnswerToLifeTheUniverseAndEverything = 42,
	};
	
	message.clear();
	stream << Numbers::Zero << LogStream::Flush();
	EXPECT_EQ(message, "0");
	
	message.clear();
	stream << Numbers::One << LogStream::Flush();
	EXPECT_EQ(message, "1");
	
	message.clear();
	stream << Numbers::Two << LogStream::Flush();
	EXPECT_EQ(message, "2");
	
	message.clear();
	stream << Numbers::Three << LogStream::Flush();
	EXPECT_EQ(message, "3");
	
	message.clear();
	stream << Numbers::TheAnswerToLifeTheUniverseAndEverything << LogStream::Flush();
	EXPECT_EQ(message, "42");
}

TEST(LogStream, CopyConstrunctor)
{
	LogChannel channel("test");
	std::string message;
	
	auto connection = channel.Connect([&message](LogEntry const& entry){
		message = entry.message;
	});

	LogStream stream(channel, LoggingLevel::Critical);
	stream << "Chuck ";
	EXPECT_TRUE(message.empty());
	
	Pomdog::LogStream stream2 = stream;
	stream2 << "Norris" << LogStream::Flush();
	EXPECT_EQ(message, "Chuck Norris");
}

TEST(LogStream, MoveConstrunctor)
{
	LogChannel channel("test");
	std::string message;
	
	auto connection = channel.Connect([&message](LogEntry const& entry){
		message = entry.message;
	});

	Pomdog::LogStream stream2 = ([&](){
		LogStream stream(channel, LoggingLevel::Critical);
		stream << "Chuck ";
		EXPECT_TRUE(message.empty());
		return std::move(stream);
	})();
	stream2 << "Norris" << LogStream::Flush();
	EXPECT_EQ(message, "Chuck Norris");
}
