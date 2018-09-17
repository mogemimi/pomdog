// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Logging/LogChannel.hpp>
#include <Pomdog/Logging/LogEntry.hpp>
#include <Pomdog/Signals/Connection.hpp>
#include <Pomdog/Signals/ScopedConnection.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::LogChannel;
using Pomdog::LogEntry;
using Pomdog::LogLevel;
using Pomdog::ScopedConnection;

TEST(LogChannel, LogLevel)
{
    static_assert(LogLevel::Critical < LogLevel::Warning, "TEST(LogChannel, LogLevel)");
    static_assert(LogLevel::Critical < LogLevel::Info, "TEST(LogChannel, LogLevel)");
    static_assert(LogLevel::Critical < LogLevel::Verbose, "TEST(LogChannel, LogLevel)");
    static_assert(LogLevel::Critical < LogLevel::Internal, "TEST(LogChannel, LogLevel)");

    static_assert(LogLevel::Warning < LogLevel::Info, "TEST(LogChannel, LogLevel)");
    static_assert(LogLevel::Warning < LogLevel::Verbose, "TEST(LogChannel, LogLevel)");
    static_assert(LogLevel::Warning < LogLevel::Internal, "TEST(LogChannel, LogLevel)");

    static_assert(LogLevel::Info < LogLevel::Verbose, "TEST(LogChannel, LogLevel)");
    static_assert(LogLevel::Info < LogLevel::Internal, "TEST(LogChannel, LogLevel)");

    static_assert(LogLevel::Verbose < LogLevel::Internal, "TEST(LogChannel, LogLevel)");
}

TEST(LogChannel, Log)
{
    LogChannel channel("test");
    std::string message;

    channel.Connect([&message](Pomdog::LogEntry const& entry){
        message = entry.Message;
    });

    channel.Log("Chuck Norris can split the atom.", LogLevel::Critical);
    EXPECT_EQ(message, "Chuck Norris can split the atom.");

    channel.Log("With his bare hands.", LogLevel::Critical);
    EXPECT_EQ(message, "With his bare hands.");
}

TEST(LogChannel, Disconnect)
{
    LogChannel channel("test");
    std::string message;

    auto connection = channel.Connect([&message](Pomdog::LogEntry const& entry){
        message = entry.Message;
    });

    channel.Log("Chuck Norris can split the atom.", LogLevel::Critical);
    EXPECT_EQ(message, "Chuck Norris can split the atom.");

    message.clear();
    connection.Disconnect();

    channel.Log("With his bare hands.", LogLevel::Critical);
    EXPECT_TRUE(message.empty());
}

TEST(LogChannel, Connection)
{
    LogChannel channel("test");
    std::string message;

    auto connectionA = channel.Connect([&message](Pomdog::LogEntry const& entry){
        message += "connection(A): ";
        message += entry.Message;
        message += ", ";
    });

    channel.Connect([&message](Pomdog::LogEntry const& entry){
        message += "connection(B): ";
        message += entry.Message;
    });

    channel.Log("Hi", LogLevel::Critical);
    EXPECT_EQ(message, "connection(A): Hi, connection(B): Hi");

    message.clear();
    connectionA.Disconnect();

    channel.Log("A disconnect", LogLevel::Critical);
    EXPECT_EQ(message, "connection(B): A disconnect");
}

TEST(LogChannel, GetName)
{
    {
        LogChannel channel("test");
        EXPECT_EQ("test", channel.GetName());
    }
    {
        LogChannel channel("Chuck Norris");
        EXPECT_EQ("Chuck Norris", channel.GetName());
    }
}

TEST(LogChannel, SetLevel)
{
    LogChannel channel("test");

    channel.SetLevel(LogLevel::Critical);
    EXPECT_EQ(LogLevel::Critical, channel.GetLevel());

    channel.SetLevel(LogLevel::Warning);
    EXPECT_EQ(LogLevel::Warning, channel.GetLevel());

    channel.SetLevel(LogLevel::Info);
    EXPECT_EQ(LogLevel::Info, channel.GetLevel());

    channel.SetLevel(LogLevel::Verbose);
    EXPECT_EQ(LogLevel::Verbose, channel.GetLevel());

    channel.SetLevel(LogLevel::Internal);
    EXPECT_EQ(LogLevel::Internal, channel.GetLevel());
}

TEST(LogChannel, VerbosityLevelThreshold)
{
    LogChannel channel("test");
    std::string message;

    channel.Connect([&message](Pomdog::LogEntry const& entry){
        message = entry.Message;
    });

    constexpr auto facts = "Chuck Norris's keyboard has an F13 key";

    auto ResetMessageAndSendLog = [&](Pomdog::LogLevel verbosity){
        message.clear();
        channel.Log(facts, verbosity);
    };

    channel.SetLevel(LogLevel::Critical);
    ResetMessageAndSendLog(LogLevel::Critical);
    EXPECT_EQ(message, facts);
    ResetMessageAndSendLog(LogLevel::Warning);
    EXPECT_TRUE(message.empty());
    ResetMessageAndSendLog(LogLevel::Info);
    EXPECT_TRUE(message.empty());
    ResetMessageAndSendLog(LogLevel::Verbose);
    EXPECT_TRUE(message.empty());
    ResetMessageAndSendLog(LogLevel::Internal);
    EXPECT_TRUE(message.empty());

    channel.SetLevel(LogLevel::Warning);
    ResetMessageAndSendLog(LogLevel::Critical);
    EXPECT_EQ(message, facts);
    ResetMessageAndSendLog(LogLevel::Warning);
    EXPECT_EQ(message, facts);
    ResetMessageAndSendLog(LogLevel::Info);
    EXPECT_TRUE(message.empty());
    ResetMessageAndSendLog(LogLevel::Verbose);
    EXPECT_TRUE(message.empty());
    ResetMessageAndSendLog(LogLevel::Internal);
    EXPECT_TRUE(message.empty());

    channel.SetLevel(LogLevel::Info);
    ResetMessageAndSendLog(LogLevel::Critical);
    EXPECT_EQ(message, facts);
    ResetMessageAndSendLog(LogLevel::Warning);
    EXPECT_EQ(message, facts);
    ResetMessageAndSendLog(LogLevel::Info);
    EXPECT_EQ(message, facts);
    ResetMessageAndSendLog(LogLevel::Verbose);
    EXPECT_TRUE(message.empty());
    ResetMessageAndSendLog(LogLevel::Internal);
    EXPECT_TRUE(message.empty());

    channel.SetLevel(LogLevel::Verbose);
    ResetMessageAndSendLog(LogLevel::Critical);
    EXPECT_EQ(message, facts);
    ResetMessageAndSendLog(LogLevel::Warning);
    EXPECT_EQ(message, facts);
    ResetMessageAndSendLog(LogLevel::Info);
    EXPECT_EQ(message, facts);
    ResetMessageAndSendLog(LogLevel::Verbose);
    EXPECT_EQ(message, facts);
    ResetMessageAndSendLog(LogLevel::Internal);
    EXPECT_TRUE(message.empty());

    channel.SetLevel(LogLevel::Internal);
    ResetMessageAndSendLog(LogLevel::Critical);
    EXPECT_EQ(message, facts);
    ResetMessageAndSendLog(LogLevel::Warning);
    EXPECT_EQ(message, facts);
    ResetMessageAndSendLog(LogLevel::Info);
    EXPECT_EQ(message, facts);
    ResetMessageAndSendLog(LogLevel::Verbose);
    EXPECT_EQ(message, facts);
    ResetMessageAndSendLog(LogLevel::Internal);
    EXPECT_EQ(message, facts);
}
