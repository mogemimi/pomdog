// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Logging/LogLevel.hpp"
#include "Pomdog/Logging/LogChannel.hpp"
#include "Pomdog/Logging/LogEntry.hpp"
#include "Pomdog/Logging/LogStream.hpp"
#include "Pomdog/Signals/Connection.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include <gtest/iutest_switch.hpp>
#include <ostream>

using Pomdog::LogChannel;
using Pomdog::LogEntry;
using Pomdog::LogLevel;
using Pomdog::LogStream;
using Pomdog::ScopedConnection;

TEST(LogStream, LogStreamFlush)
{
    LogChannel channel("test");
    LogStream stream(channel, LogLevel::Critical);

    std::string message;
    auto connection = channel.Connect([&message](LogEntry const& entry){
        message = entry.Message;
    });

    message.clear();
    stream << "Chuck " << "Norris " << "Approval.";
    stream.Flush();
    EXPECT_EQ(message, "Chuck Norris Approval.");

    message.clear();
    stream << "Chuck ";
    stream << "Norris ";
    stream << "makes ";
    stream << "onions ";
    stream << "cry!";
    EXPECT_TRUE(message.empty());
    stream.Flush();
    EXPECT_EQ(message, "Chuck Norris makes onions cry!");
}

TEST(LogStream, EnumerationToString)
{
    LogChannel channel("test");
    LogStream stream(channel, LogLevel::Critical);

    std::string message;
    auto connection = channel.Connect([&message](LogEntry const& entry){
        message = entry.Message;
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
    stream << Numbers::Zero;
    stream.Flush();
    EXPECT_EQ(message, "0");

    message.clear();
    stream << Numbers::One;
    stream.Flush();
    EXPECT_EQ(message, "1");

    message.clear();
    stream << Numbers::Two;
    stream.Flush();
    EXPECT_EQ(message, "2");

    message.clear();
    stream << Numbers::Three;
    stream.Flush();
    EXPECT_EQ(message, "3");

    message.clear();
    stream << Numbers::TheAnswerToLifeTheUniverseAndEverything;
    stream.Flush();
    EXPECT_EQ(message, "42");
}

TEST(LogStream, CopyConstrunctor)
{
    LogChannel channel("test");
    std::string message;

    auto connection = channel.Connect([&message](LogEntry const& entry){
        message = entry.Message;
    });

    LogStream stream(channel, LogLevel::Critical);
    stream << "Chuck ";
    EXPECT_TRUE(message.empty());

    Pomdog::LogStream stream2 = stream;
    stream2 << "Norris";
    stream2.Flush();
    EXPECT_EQ(message, "Chuck Norris");
}

TEST(LogStream, MoveConstrunctor)
{
    LogChannel channel("test");
    std::string message;

    auto connection = channel.Connect([&message](LogEntry const& entry){
        message = entry.Message;
    });

    Pomdog::LogStream stream2 = ([&](){
        LogStream stream(channel, LogLevel::Critical);
        stream << "Chuck ";
        EXPECT_TRUE(message.empty());
        return std::move(stream);
    })();
    stream2 << "Norris";
    stream2.Flush();
    EXPECT_EQ(message, "Chuck Norris");
}
