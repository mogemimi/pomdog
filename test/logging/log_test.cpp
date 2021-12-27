// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/logging/log.h"
#include "pomdog/logging/log_channel.h"
#include "pomdog/logging/log_entry.h"
#include "pomdog/signals/scoped_connection.h"
#include <catch_amalgamated.hpp>
#include <vector>

using pomdog::Log;
using pomdog::LogChannel;
using pomdog::LogEntry;
using pomdog::LogLevel;
using pomdog::ScopedConnection;

TEST_CASE("Log", "[Log]")
{
    std::vector<std::string> messages;
    std::vector<std::string> tags;
    std::vector<LogLevel> levels;

    messages.clear();
    tags.clear();
    levels.clear();
    Log::SetLevel(pomdog::LogLevel::Verbose);

    SECTION("FirstCase")
    {
        ScopedConnection connection = Log::Connect([&](const LogEntry& entry) {
            messages.push_back(entry.Message);
            tags.push_back(entry.Tag);
            levels.push_back(entry.Verbosity);
        });

        Log::Info("Hello");
        Log::Info("Hi");

        REQUIRE(messages.size() == 2);
        REQUIRE(messages[0] == "Hello");
        REQUIRE(messages[1] == "Hi");
        REQUIRE(tags.size() == 2);
        REQUIRE(tags[0].empty());
        REQUIRE(tags[1].empty());
    }
    SECTION("ConnectToChannel")
    {
        ScopedConnection connection = Log::Connect("#TestChannel", [&](const LogEntry& entry) {
            messages.push_back(entry.Message);
            tags.push_back(entry.Tag);
            levels.push_back(entry.Verbosity);
        });

        Log::Info("Nyan1");
        Log::Info("#TestChannel", "Nyan2");
        Log::Info("#TestChannel", "Nyan3");
        Log::Info("#ABC", "Nyan4");

        REQUIRE(messages.size() == 2);
        REQUIRE(messages[0] == "Nyan2");
        REQUIRE(messages[1] == "Nyan3");
        REQUIRE(tags.size() == 2);
        REQUIRE(tags[0] == "#TestChannel");
        REQUIRE(tags[1] == "#TestChannel");
    }
    SECTION("ConnectToChannel2")
    {
        ScopedConnection connectionDog = Log::Connect("#Dog", [&](const LogEntry& entry) {
            messages.push_back(entry.Message + "(in Dog)");
            tags.push_back(entry.Tag + "(in Dog)");
            levels.push_back(entry.Verbosity);
        });

        ScopedConnection connectionCat = Log::Connect("#Cat", [&](const LogEntry& entry) {
            messages.push_back(entry.Message + "(in Cat)");
            tags.push_back(entry.Tag + "(in Cat)");
            levels.push_back(entry.Verbosity);
        });

        Log::Info("Nyan1");
        Log::Info("#Dog", "Nyan2");
        Log::Info("#Cat", "Nyan3");
        Log::Info("#ABC", "Nyan4");

        REQUIRE(messages.size() == 2);
        REQUIRE(messages[0] == "Nyan2(in Dog)");
        REQUIRE(messages[1] == "Nyan3(in Cat)");
        REQUIRE(tags.size() == 2);
        REQUIRE(tags[0] == "#Dog(in Dog)");
        REQUIRE(tags[1] == "#Cat(in Cat)");
    }
    SECTION("ConnectToDefaultChannel")
    {
        ScopedConnection connection = Log::Connect([&](const LogEntry& entry) {
            messages.push_back(entry.Message);
            tags.push_back(entry.Tag);
            levels.push_back(entry.Verbosity);
        });

        ScopedConnection connectionCat = Log::Connect("#Cat", [&](const LogEntry& entry) {
            messages.push_back(entry.Message + "(in Cat)");
            tags.push_back(entry.Tag + "(in Cat)");
            levels.push_back(entry.Verbosity);
        });

        Log::Info("Nyan1");
        Log::Info("#Dog", "Nyan2");
        Log::Info("#Cat", "Nyan3");

        REQUIRE(messages.size() == 4);
        REQUIRE(messages[0] == "Nyan1");
        REQUIRE(messages[1] == "Nyan2");
        REQUIRE(messages[2] == "Nyan3");
        REQUIRE(messages[3] == "Nyan3(in Cat)");
        REQUIRE(tags.size() == 4);
        REQUIRE(tags[0].empty());
        REQUIRE(tags[1] == "#Dog");
        REQUIRE(tags[2] == "#Cat");
        REQUIRE(tags[3] == "#Cat(in Cat)");
    }
    SECTION("SetVerbosityLevels")
    {
        ScopedConnection connection = Log::Connect("TestChannel", [&](const LogEntry& entry) {
            messages.push_back(entry.Message);
            tags.push_back(entry.Tag);
            levels.push_back(entry.Verbosity);
        });

        Log::SetLevel("TestChannel", LogLevel::Internal);
        Log::Internal("TestChannel", "(A) Internal");
        Log::Verbose("TestChannel",  "(A) Verbose");
        Log::Info("TestChannel",     "(A) Info");
        Log::Warning("TestChannel",  "(A) Warning");
        Log::Critical("TestChannel", "(A) Critical");
        REQUIRE(LogLevel::Internal == Log::GetLevel("TestChannel"));

        Log::SetLevel("TestChannel", LogLevel::Verbose);
        Log::Internal("TestChannel", "(B) Internal");
        Log::Verbose("TestChannel",  "(B) Verbose");
        Log::Info("TestChannel",     "(B) Info");
        Log::Warning("TestChannel",  "(B) Warning");
        Log::Critical("TestChannel", "(B) Critical");
        REQUIRE(LogLevel::Verbose == Log::GetLevel("TestChannel"));

        Log::SetLevel("TestChannel", LogLevel::Info);
        Log::Internal("TestChannel", "(C) Internal");
        Log::Verbose("TestChannel",  "(C) Verbose");
        Log::Info("TestChannel",     "(C) Info");
        Log::Warning("TestChannel",  "(C) Warning");
        Log::Critical("TestChannel", "(C) Critical");
        REQUIRE(LogLevel::Info == Log::GetLevel("TestChannel"));

        Log::SetLevel("TestChannel", LogLevel::Warning);
        Log::Internal("TestChannel", "(D) Internal");
        Log::Verbose("TestChannel",  "(D) Verbose");
        Log::Info("TestChannel",     "(D) Info");
        Log::Warning("TestChannel",  "(D) Warning");
        Log::Critical("TestChannel", "(D) Critical");
        REQUIRE(LogLevel::Warning == Log::GetLevel("TestChannel"));

        Log::SetLevel("TestChannel", LogLevel::Critical);
        Log::Internal("TestChannel", "(E) Internal");
        Log::Verbose("TestChannel",  "(E) Verbose");
        Log::Info("TestChannel",     "(E) Info");
        Log::Warning("TestChannel",  "(E) Warning");
        Log::Critical("TestChannel", "(E) Critical");
        REQUIRE(LogLevel::Critical == Log::GetLevel("TestChannel"));

        REQUIRE(messages.size() == 15);
        REQUIRE(messages[0] == "(A) Internal");
        REQUIRE(messages[1] == "(A) Verbose");
        REQUIRE(messages[2] == "(A) Info");
        REQUIRE(messages[3] == "(A) Warning");
        REQUIRE(messages[4] == "(A) Critical");
        REQUIRE(messages[5] == "(B) Verbose");
        REQUIRE(messages[6] == "(B) Info");
        REQUIRE(messages[7] == "(B) Warning");
        REQUIRE(messages[8] == "(B) Critical");
        REQUIRE(messages[9] == "(C) Info");
        REQUIRE(messages[10] == "(C) Warning");
        REQUIRE(messages[11] == "(C) Critical");
        REQUIRE(messages[12] == "(D) Warning");
        REQUIRE(messages[13] == "(D) Critical");
        REQUIRE(messages[14] == "(E) Critical");

        REQUIRE(levels.size() == 15);
        REQUIRE(levels[0] == LogLevel::Internal);
        REQUIRE(levels[1] == LogLevel::Verbose);
        REQUIRE(levels[2] == LogLevel::Info);
        REQUIRE(levels[3] == LogLevel::Warning);
        REQUIRE(levels[4] == LogLevel::Critical);
        REQUIRE(levels[5] == LogLevel::Verbose);
        REQUIRE(levels[6] == LogLevel::Info);
        REQUIRE(levels[7] == LogLevel::Warning);
        REQUIRE(levels[8] == LogLevel::Critical);
        REQUIRE(levels[9] == LogLevel::Info);
        REQUIRE(levels[10] == LogLevel::Warning);
        REQUIRE(levels[11] == LogLevel::Critical);
        REQUIRE(levels[12] == LogLevel::Warning);
        REQUIRE(levels[13] == LogLevel::Critical);
        REQUIRE(levels[14] == LogLevel::Critical);
    }
    SECTION("SendToUserChannels")
    {
        auto handler = [&](const LogEntry& entry) {
            messages.push_back(entry.Message);
            tags.push_back(entry.Tag);
        };

        ScopedConnection connection1 = Log::Connect("#Test1", handler);
        ScopedConnection connection2 = Log::Connect("#Test2", handler);

        Log::SetLevel("Test1", LogLevel::Verbose);
        Log::SetLevel("Test2", LogLevel::Verbose);

        Log::Info("(A) Send to default channel");
        Log::Info("#Test1", "(B) Send to channel Test1");
        Log::Info("#Test2", "(C) Send to channel Test2");
        Log::Info("#NyanNyanCat", "(D) Send to nonexistent channel");

        // Connect
        ScopedConnection connection3 = Log::Connect("#NyanNyanCat", handler);
        Log::SetLevel("#NyanNyanCat", LogLevel::Verbose);

        Log::Info("#NyanNyanCat", "(E) Send to new channel");

        // Disconnect
        connection1.Disconnect();
        Log::Info("#Test1", "(F) Send to channel Test1");
        Log::Info("#Test2", "(G) Send to channel Test2");

        REQUIRE(messages.size() == 4);
        REQUIRE(messages[0] == "(B) Send to channel Test1");
        REQUIRE(messages[1] == "(C) Send to channel Test2");
        REQUIRE(messages[2] == "(E) Send to new channel");
        REQUIRE(messages[3] == "(G) Send to channel Test2");

        REQUIRE(tags.size() == 4);
        REQUIRE(tags[0] == "#Test1");
        REQUIRE(tags[1] == "#Test2");
        REQUIRE(tags[2] == "#NyanNyanCat");
        REQUIRE(tags[3] == "#Test2");
    }
    SECTION("CallToDisconnectInCallback")
    {
        ScopedConnection connectionA, connectionB;

        connectionA = Log::Connect([&](const LogEntry& entry) {
            messages.push_back(entry.Message + " at A");

            if (entry.Message == "Disconnect B") {
                connectionB.Disconnect();
            }
        });

        connectionB = Log::Connect([&](const LogEntry& entry) {
            messages.push_back(entry.Message + " at B");
        });

        Log::Info("Hello");
        Log::Info("Hi");
        Log::Info("Disconnect B");
        Log::Info("Chuck Norris");

        REQUIRE(messages.size() == 6);
        REQUIRE(messages[0] == "Hello at A");
        REQUIRE(messages[1] == "Hello at B");
        REQUIRE(messages[2] == "Hi at A");
        REQUIRE(messages[3] == "Hi at B");
        REQUIRE(messages[4] == "Disconnect B at A");
        REQUIRE(messages[5] == "Chuck Norris at A");
    }
    SECTION("CallToDisconnectSelfInCallback")
    {
        ScopedConnection connectionA;

        connectionA = Log::Connect([&](const LogEntry& entry) {
            messages.push_back(entry.Message + " at A");

            if (entry.Message == "Disconnect") {
                connectionA.Disconnect();
            }
        });

        Log::Info("Hello");
        Log::Info("Hi");
        Log::Info("Disconnect");
        Log::Info("Chuck Norris");

        REQUIRE(messages.size() == 3);
        REQUIRE(messages[0] == "Hello at A");
        REQUIRE(messages[1] == "Hi at A");
        REQUIRE(messages[2] == "Disconnect at A");
    }
}

TEST_CASE("Log::SetLevel", "[Log]")
{
    auto verbosity = Log::GetLevel();

    Log::SetLevel(LogLevel::Internal);
    REQUIRE(LogLevel::Internal == Log::GetLevel());

    Log::SetLevel(LogLevel::Verbose);
    REQUIRE(LogLevel::Verbose == Log::GetLevel());

    Log::SetLevel(LogLevel::Info);
    REQUIRE(LogLevel::Info == Log::GetLevel());

    Log::SetLevel(LogLevel::Warning);
    REQUIRE(LogLevel::Warning == Log::GetLevel());

    Log::SetLevel(LogLevel::Critical);
    REQUIRE(LogLevel::Critical == Log::GetLevel());

    Log::SetLevel(verbosity);
}
