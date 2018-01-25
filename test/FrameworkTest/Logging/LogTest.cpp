// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Logging/Log.hpp>
#include <Pomdog/Logging/LogChannel.hpp>
#include <Pomdog/Logging/LogEntry.hpp>
#include <Pomdog/Signals/Event.hpp>
#include <Pomdog/Signals/ScopedConnection.hpp>
#include <gtest/iutest_switch.hpp>
#include <vector>

using Pomdog::Log;
using Pomdog::LogChannel;
using Pomdog::LogEntry;
using Pomdog::LogLevel;
using Pomdog::ScopedConnection;

struct LogTest: public ::testing::Test
{
    std::vector<std::string> messages;
    std::vector<std::string> tags;
    std::vector<LogLevel> levels;

    void SetUp() override
    {
        messages.clear();
        tags.clear();
        levels.clear();
        Log::SetLevel(Pomdog::LogLevel::Verbose);
    }
};

TEST_F(LogTest, FirstCase)
{
    ScopedConnection connection = Log::Connect([&](LogEntry const& entry) {
        messages.push_back(entry.Message);
        tags.push_back(entry.Tag);
        levels.push_back(entry.Verbosity);
    });

    Log::Info("Hello");
    Log::Info("Hi");

    ASSERT_EQ(2, messages.size());
    EXPECT_EQ("Hello", messages[0]);
    EXPECT_EQ("Hi", messages[1]);
    ASSERT_EQ(2, tags.size());
    EXPECT_TRUE(tags[0].empty());
    EXPECT_TRUE(tags[1].empty());
}

TEST_F(LogTest, ConnectToChannel)
{
    ScopedConnection connection = Log::Connect("#TestChannel", [&](LogEntry const& entry) {
        messages.push_back(entry.Message);
        tags.push_back(entry.Tag);
        levels.push_back(entry.Verbosity);
    });

    Log::Info("Nyan1");
    Log::Info("#TestChannel", "Nyan2");
    Log::Info("#TestChannel", "Nyan3");
    Log::Info("#ABC", "Nyan4");

    ASSERT_EQ(2, messages.size());
    EXPECT_EQ("Nyan2", messages[0]);
    EXPECT_EQ("Nyan3", messages[1]);
    ASSERT_EQ(2, tags.size());
    EXPECT_EQ("#TestChannel", tags[0]);
    EXPECT_EQ("#TestChannel", tags[1]);
}

TEST_F(LogTest, ConnectToChannel2)
{
    ScopedConnection connectionDog = Log::Connect("#Dog", [&](LogEntry const& entry) {
        messages.push_back(entry.Message + "(in Dog)");
        tags.push_back(entry.Tag + "(in Dog)");
        levels.push_back(entry.Verbosity);
    });

    ScopedConnection connectionCat = Log::Connect("#Cat", [&](LogEntry const& entry) {
        messages.push_back(entry.Message + "(in Cat)");
        tags.push_back(entry.Tag + "(in Cat)");
        levels.push_back(entry.Verbosity);
    });

    Log::Info("Nyan1");
    Log::Info("#Dog", "Nyan2");
    Log::Info("#Cat", "Nyan3");
    Log::Info("#ABC", "Nyan4");

    ASSERT_EQ(2, messages.size());
    EXPECT_EQ("Nyan2(in Dog)", messages[0]);
    EXPECT_EQ("Nyan3(in Cat)", messages[1]);
    ASSERT_EQ(2, tags.size());
    EXPECT_EQ("#Dog(in Dog)", tags[0]);
    EXPECT_EQ("#Cat(in Cat)", tags[1]);
}

TEST_F(LogTest, ConnectToDefaultChannel)
{
    ScopedConnection connection = Log::Connect([&](LogEntry const& entry) {
        messages.push_back(entry.Message);
        tags.push_back(entry.Tag);
        levels.push_back(entry.Verbosity);
    });

    ScopedConnection connectionCat = Log::Connect("#Cat", [&](LogEntry const& entry) {
        messages.push_back(entry.Message + "(in Cat)");
        tags.push_back(entry.Tag + "(in Cat)");
        levels.push_back(entry.Verbosity);
    });

    Log::Info("Nyan1");
    Log::Info("#Dog", "Nyan2");
    Log::Info("#Cat", "Nyan3");

    ASSERT_EQ(4, messages.size());
    EXPECT_EQ("Nyan1", messages[0]);
    EXPECT_EQ("Nyan2", messages[1]);
    EXPECT_EQ("Nyan3", messages[2]);
    EXPECT_EQ("Nyan3(in Cat)", messages[3]);
    ASSERT_EQ(4, tags.size());
    EXPECT_TRUE(tags[0].empty());
    EXPECT_EQ("#Dog", tags[1]);
    EXPECT_EQ("#Cat", tags[2]);
    EXPECT_EQ("#Cat(in Cat)", tags[3]);
}

TEST(Log, SetDefaultLevels)
{
    auto verbosity = Log::GetLevel();

    Log::SetLevel(LogLevel::Internal);
    EXPECT_EQ(LogLevel::Internal, Log::GetLevel());

    Log::SetLevel(LogLevel::Verbose);
    EXPECT_EQ(LogLevel::Verbose, Log::GetLevel());

    Log::SetLevel(LogLevel::Info);
    EXPECT_EQ(LogLevel::Info, Log::GetLevel());

    Log::SetLevel(LogLevel::Warning);
    EXPECT_EQ(LogLevel::Warning, Log::GetLevel());

    Log::SetLevel(LogLevel::Critical);
    EXPECT_EQ(LogLevel::Critical, Log::GetLevel());

    Log::SetLevel(verbosity);
}

TEST_F(LogTest, SetVerbosityLevels)
{
    ScopedConnection connection = Log::Connect("TestChannel", [&](LogEntry const& entry) {
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
    EXPECT_EQ(LogLevel::Internal, Log::GetLevel("TestChannel"));

    Log::SetLevel("TestChannel", LogLevel::Verbose);
    Log::Internal("TestChannel", "(B) Internal");
    Log::Verbose("TestChannel",  "(B) Verbose");
    Log::Info("TestChannel",     "(B) Info");
    Log::Warning("TestChannel",  "(B) Warning");
    Log::Critical("TestChannel", "(B) Critical");
    EXPECT_EQ(LogLevel::Verbose, Log::GetLevel("TestChannel"));

    Log::SetLevel("TestChannel", LogLevel::Info);
    Log::Internal("TestChannel", "(C) Internal");
    Log::Verbose("TestChannel",  "(C) Verbose");
    Log::Info("TestChannel",     "(C) Info");
    Log::Warning("TestChannel",  "(C) Warning");
    Log::Critical("TestChannel", "(C) Critical");
    EXPECT_EQ(LogLevel::Info, Log::GetLevel("TestChannel"));

    Log::SetLevel("TestChannel", LogLevel::Warning);
    Log::Internal("TestChannel", "(D) Internal");
    Log::Verbose("TestChannel",  "(D) Verbose");
    Log::Info("TestChannel",     "(D) Info");
    Log::Warning("TestChannel",  "(D) Warning");
    Log::Critical("TestChannel", "(D) Critical");
    EXPECT_EQ(LogLevel::Warning, Log::GetLevel("TestChannel"));

    Log::SetLevel("TestChannel", LogLevel::Critical);
    Log::Internal("TestChannel", "(E) Internal");
    Log::Verbose("TestChannel",  "(E) Verbose");
    Log::Info("TestChannel",     "(E) Info");
    Log::Warning("TestChannel",  "(E) Warning");
    Log::Critical("TestChannel", "(E) Critical");
    EXPECT_EQ(LogLevel::Critical, Log::GetLevel("TestChannel"));

    ASSERT_EQ(15, messages.size());
    EXPECT_EQ("(A) Internal", messages[0]);
    EXPECT_EQ("(A) Verbose", messages[1]);
    EXPECT_EQ("(A) Info", messages[2]);
    EXPECT_EQ("(A) Warning", messages[3]);
    EXPECT_EQ("(A) Critical", messages[4]);
    EXPECT_EQ("(B) Verbose", messages[5]);
    EXPECT_EQ("(B) Info", messages[6]);
    EXPECT_EQ("(B) Warning", messages[7]);
    EXPECT_EQ("(B) Critical", messages[8]);
    EXPECT_EQ("(C) Info", messages[9]);
    EXPECT_EQ("(C) Warning", messages[10]);
    EXPECT_EQ("(C) Critical", messages[11]);
    EXPECT_EQ("(D) Warning", messages[12]);
    EXPECT_EQ("(D) Critical", messages[13]);
    EXPECT_EQ("(E) Critical", messages[14]);

    ASSERT_EQ(15, levels.size());
    EXPECT_EQ(LogLevel::Internal, levels[0]);
    EXPECT_EQ(LogLevel::Verbose, levels[1]);
    EXPECT_EQ(LogLevel::Info, levels[2]);
    EXPECT_EQ(LogLevel::Warning, levels[3]);
    EXPECT_EQ(LogLevel::Critical, levels[4]);
    EXPECT_EQ(LogLevel::Verbose, levels[5]);
    EXPECT_EQ(LogLevel::Info, levels[6]);
    EXPECT_EQ(LogLevel::Warning, levels[7]);
    EXPECT_EQ(LogLevel::Critical, levels[8]);
    EXPECT_EQ(LogLevel::Info, levels[9]);
    EXPECT_EQ(LogLevel::Warning, levels[10]);
    EXPECT_EQ(LogLevel::Critical, levels[11]);
    EXPECT_EQ(LogLevel::Warning, levels[12]);
    EXPECT_EQ(LogLevel::Critical, levels[13]);
    EXPECT_EQ(LogLevel::Critical, levels[14]);
}

TEST_F(LogTest, SendToUserChannels)
{
    auto handler = [&](LogEntry const& entry){
        messages.push_back(entry.Message);
        tags.push_back(entry.Tag);
    };

    ScopedConnection connection1(Log::Connect("#Test1", handler));
    ScopedConnection connection2(Log::Connect("#Test2", handler));

    Log::SetLevel("Test1", LogLevel::Verbose);
    Log::SetLevel("Test2", LogLevel::Verbose);

    Log::Info("(A) Send to default channel");
    Log::Info("#Test1", "(B) Send to channel Test1");
    Log::Info("#Test2", "(C) Send to channel Test2");
    Log::Info("#NyanNyanCat", "(D) Send to nonexistent channel");

    // Connect
    ScopedConnection connection3(Log::Connect("#NyanNyanCat", handler));
    Log::SetLevel("#NyanNyanCat", LogLevel::Verbose);

    Log::Info("#NyanNyanCat", "(E) Send to new channel");

    // Disconnect
    connection1.Disconnect();
    Log::Info("#Test1", "(F) Send to channel Test1");
    Log::Info("#Test2", "(G) Send to channel Test2");

    ASSERT_EQ(4, messages.size());
    EXPECT_EQ(messages[0], "(B) Send to channel Test1");
    EXPECT_EQ(messages[1], "(C) Send to channel Test2");
    EXPECT_EQ(messages[2], "(E) Send to new channel");
    EXPECT_EQ(messages[3], "(G) Send to channel Test2");

    ASSERT_EQ(4, tags.size());
    EXPECT_EQ(tags[0], "#Test1");
    EXPECT_EQ(tags[1], "#Test2");
    EXPECT_EQ(tags[2], "#NyanNyanCat");
    EXPECT_EQ(tags[3], "#Test2");
}

TEST_F(LogTest, CallToDisconnectInCallback)
{
    ScopedConnection connectionA, connectionB;

    connectionA = Log::Connect([&](LogEntry const& entry){
        messages.push_back(entry.Message + " at A");

        if (entry.Message == "Disconnect B") {
            connectionB.Disconnect();
        }
    });

    connectionB = Log::Connect([&](LogEntry const& entry){
        messages.push_back(entry.Message + " at B");
    });

    Log::Info("Hello");
    Log::Info("Hi");
    Log::Info("Disconnect B");
    Log::Info("Chuck Norris");

    ASSERT_EQ(6, messages.size());
    EXPECT_EQ(messages[0], "Hello at A");
    EXPECT_EQ(messages[1], "Hello at B");
    EXPECT_EQ(messages[2], "Hi at A");
    EXPECT_EQ(messages[3], "Hi at B");
    EXPECT_EQ(messages[4], "Disconnect B at A");
    EXPECT_EQ(messages[5], "Chuck Norris at A");
}

TEST_F(LogTest, CallToDisconnectSelfInCallback)
{
    ScopedConnection connectionA;

    connectionA = Log::Connect([&](LogEntry const& entry){
        messages.push_back(entry.Message + " at A");

        if (entry.Message == "Disconnect") {
            connectionA.Disconnect();
        }
    });

    Log::Info("Hello");
    Log::Info("Hi");
    Log::Info("Disconnect");
    Log::Info("Chuck Norris");

    ASSERT_EQ(3, messages.size());
    EXPECT_EQ(messages[0], "Hello at A");
    EXPECT_EQ(messages[1], "Hi at A");
    EXPECT_EQ(messages[2], "Disconnect at A");
}
