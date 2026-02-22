// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/logging/log_channel.h"
#include "pomdog/logging/log_entry.h"
#include "pomdog/signals/connection.h"
#include "pomdog/signals/scoped_connection.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <ostream>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::LogChannel;
using pomdog::LogEntry;
using pomdog::LogLevel;
using pomdog::ScopedConnection;

TEST_CASE("LogChannel")
{
    SUBCASE("LogLevel ordering")
    {
        REQUIRE(LogLevel::Critical < LogLevel::Warning);
        REQUIRE(LogLevel::Critical < LogLevel::Info);
        REQUIRE(LogLevel::Critical < LogLevel::Verbose);
        REQUIRE(LogLevel::Critical < LogLevel::Internal);

        REQUIRE(LogLevel::Warning < LogLevel::Info);
        REQUIRE(LogLevel::Warning < LogLevel::Verbose);
        REQUIRE(LogLevel::Warning < LogLevel::Internal);

        REQUIRE(LogLevel::Info < LogLevel::Verbose);
        REQUIRE(LogLevel::Info < LogLevel::Internal);

        REQUIRE(LogLevel::Verbose < LogLevel::Internal);
    }
    SUBCASE("Log")
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
    SUBCASE("disconnect")
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
    SUBCASE("Connection")
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
    SUBCASE("GetName")
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
    SUBCASE("SetLevel")
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
    SUBCASE("verbosity level threshold")
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
