// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/logging/log_channel.h"
#include "pomdog/logging/log_entry.h"
#include "pomdog/signals/connection.h"
#include "pomdog/signals/scoped_connection.h"
#include <catch_amalgamated.hpp>

using pomdog::LogChannel;
using pomdog::LogEntry;
using pomdog::LogLevel;
using pomdog::ScopedConnection;

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

        auto conn = channel.Connect([&](const LogEntry& entry) {
            message = entry.Message;
        });

        channel.Log("Chuck Norris can split the atom.", LogLevel::Critical);
        REQUIRE(message == "Chuck Norris can split the atom.");

        channel.Log("With his bare hands.", LogLevel::Critical);
        REQUIRE(message == "With his bare hands.");

        conn.disconnect();
    }
    SECTION("Disconnect")
    {
        LogChannel channel("test");
        std::string message;

        auto connection = channel.Connect([&](const LogEntry& entry) {
            message = entry.Message;
        });

        channel.Log("Chuck Norris can split the atom.", LogLevel::Critical);
        REQUIRE(message == "Chuck Norris can split the atom.");

        message.clear();
        connection.disconnect();

        channel.Log("With his bare hands.", LogLevel::Critical);
        REQUIRE(message.empty());
    }
    SECTION("Connection")
    {
        LogChannel channel("test");
        std::string message;

        auto connA = channel.Connect([&](const LogEntry& entry) {
            message += "connection(A): ";
            message += entry.Message;
            message += ", ";
        });

        auto connB = channel.Connect([&](const LogEntry& entry) {
            message += "connection(B): ";
            message += entry.Message;
        });

        channel.Log("Hi", LogLevel::Critical);
        REQUIRE(message == "connection(A): Hi, connection(B): Hi");

        message.clear();
        connA.disconnect();

        channel.Log("A disconnect", LogLevel::Critical);
        REQUIRE(message == "connection(B): A disconnect");

        connB.disconnect();
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

        auto conn = channel.Connect([&](const LogEntry& entry) {
            message = entry.Message;
        });

        constexpr std::string_view facts = "Chuck Norris's keyboard has an F13 key";

        auto ResetMessageAndSendLog = [&](LogLevel verbosity) {
            message.clear();
            channel.Log(std::string(facts), verbosity);
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

        conn.disconnect();
    }
}
