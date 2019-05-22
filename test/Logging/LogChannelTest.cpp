// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Logging/LogChannel.hpp"
#include "Pomdog/Logging/LogEntry.hpp"
#include "Pomdog/Signals/Connection.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "catch.hpp"

using Pomdog::LogChannel;
using Pomdog::LogEntry;
using Pomdog::LogLevel;
using Pomdog::ScopedConnection;

TEST_CASE("LogChannel", "[LogChannel]")
{
    SECTION("LogLevel")
    {
        STATIC_REQUIRE(LogLevel::Critical < LogLevel::Warning);
        STATIC_REQUIRE(LogLevel::Critical < LogLevel::Info);
        STATIC_REQUIRE(LogLevel::Critical < LogLevel::Verbose);
        STATIC_REQUIRE(LogLevel::Critical < LogLevel::Internal);

        STATIC_REQUIRE(LogLevel::Warning < LogLevel::Info);
        STATIC_REQUIRE(LogLevel::Warning < LogLevel::Verbose);
        STATIC_REQUIRE(LogLevel::Warning < LogLevel::Internal);

        STATIC_REQUIRE(LogLevel::Info < LogLevel::Verbose);
        STATIC_REQUIRE(LogLevel::Info < LogLevel::Internal);

        STATIC_REQUIRE(LogLevel::Verbose < LogLevel::Internal);
    }
    SECTION("LogChannel::Log")
    {
        LogChannel channel("test");
        std::string message;

        channel.Connect([&message](Pomdog::LogEntry const& entry){
            message = entry.Message;
        });

        channel.Log("Chuck Norris can split the atom.", LogLevel::Critical);
        REQUIRE(message == "Chuck Norris can split the atom.");

        channel.Log("With his bare hands.", LogLevel::Critical);
        REQUIRE(message == "With his bare hands.");
    }
    SECTION("Disconnect")
    {
        LogChannel channel("test");
        std::string message;

        auto connection = channel.Connect([&message](Pomdog::LogEntry const& entry){
            message = entry.Message;
        });

        channel.Log("Chuck Norris can split the atom.", LogLevel::Critical);
        REQUIRE(message == "Chuck Norris can split the atom.");

        message.clear();
        connection.Disconnect();

        channel.Log("With his bare hands.", LogLevel::Critical);
        REQUIRE(message.empty());
    }
    SECTION("Connection")
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
        REQUIRE(message == "connection(A): Hi, connection(B): Hi");

        message.clear();
        connectionA.Disconnect();

        channel.Log("A disconnect", LogLevel::Critical);
        REQUIRE(message == "connection(B): A disconnect");
    }
    SECTION("GetName")
    {
        {
            LogChannel channel("test");
            REQUIRE(channel.GetName() == "test");
        }
        {
            LogChannel channel("Chuck Norris");
            REQUIRE(channel.GetName() == "Chuck Norris");
        }
    }
    SECTION("SetLevel")
    {
        LogChannel channel("test");

        channel.SetLevel(LogLevel::Critical);
        REQUIRE(LogLevel::Critical == channel.GetLevel());

        channel.SetLevel(LogLevel::Warning);
        REQUIRE(LogLevel::Warning == channel.GetLevel());

        channel.SetLevel(LogLevel::Info);
        REQUIRE(LogLevel::Info == channel.GetLevel());

        channel.SetLevel(LogLevel::Verbose);
        REQUIRE(LogLevel::Verbose == channel.GetLevel());

        channel.SetLevel(LogLevel::Internal);
        REQUIRE(LogLevel::Internal == channel.GetLevel());
    }
    SECTION("Verbosity level threshold")
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
        REQUIRE(message == facts);
        ResetMessageAndSendLog(LogLevel::Warning);
        REQUIRE(message.empty());
        ResetMessageAndSendLog(LogLevel::Info);
        REQUIRE(message.empty());
        ResetMessageAndSendLog(LogLevel::Verbose);
        REQUIRE(message.empty());
        ResetMessageAndSendLog(LogLevel::Internal);
        REQUIRE(message.empty());

        channel.SetLevel(LogLevel::Warning);
        ResetMessageAndSendLog(LogLevel::Critical);
        REQUIRE(message == facts);
        ResetMessageAndSendLog(LogLevel::Warning);
        REQUIRE(message == facts);
        ResetMessageAndSendLog(LogLevel::Info);
        REQUIRE(message.empty());
        ResetMessageAndSendLog(LogLevel::Verbose);
        REQUIRE(message.empty());
        ResetMessageAndSendLog(LogLevel::Internal);
        REQUIRE(message.empty());

        channel.SetLevel(LogLevel::Info);
        ResetMessageAndSendLog(LogLevel::Critical);
        REQUIRE(message == facts);
        ResetMessageAndSendLog(LogLevel::Warning);
        REQUIRE(message == facts);
        ResetMessageAndSendLog(LogLevel::Info);
        REQUIRE(message == facts);
        ResetMessageAndSendLog(LogLevel::Verbose);
        REQUIRE(message.empty());
        ResetMessageAndSendLog(LogLevel::Internal);
        REQUIRE(message.empty());

        channel.SetLevel(LogLevel::Verbose);
        ResetMessageAndSendLog(LogLevel::Critical);
        REQUIRE(message == facts);
        ResetMessageAndSendLog(LogLevel::Warning);
        REQUIRE(message == facts);
        ResetMessageAndSendLog(LogLevel::Info);
        REQUIRE(message == facts);
        ResetMessageAndSendLog(LogLevel::Verbose);
        REQUIRE(message == facts);
        ResetMessageAndSendLog(LogLevel::Internal);
        REQUIRE(message.empty());

        channel.SetLevel(LogLevel::Internal);
        ResetMessageAndSendLog(LogLevel::Critical);
        REQUIRE(message == facts);
        ResetMessageAndSendLog(LogLevel::Warning);
        REQUIRE(message == facts);
        ResetMessageAndSendLog(LogLevel::Info);
        REQUIRE(message == facts);
        ResetMessageAndSendLog(LogLevel::Verbose);
        REQUIRE(message == facts);
        ResetMessageAndSendLog(LogLevel::Internal);
        REQUIRE(message == facts);
    }
}
