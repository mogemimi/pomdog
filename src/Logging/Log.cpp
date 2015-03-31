// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Logging/LogEntry.hpp"
#include "Pomdog/Logging/LogStream.hpp"
#include "Pomdog/Logging/LogChannel.hpp"
#include "Pomdog/Event/Connection.hpp"
#include "Pomdog/Event/ScopedConnection.hpp"
#include <algorithm>
#include <memory>
#include <mutex>
#include <vector>

namespace Pomdog {
namespace {

class Logger {
public:
    Connection Connect(std::function<void(LogEntry const&)> const& slot);

    Connection Connect(std::function<void(LogEntry const&)> && slot);

    Connection Connect(std::string const& channelName, std::function<void(LogEntry const&)> const& slot);

    Connection Connect(std::string const& channelName, std::function<void(LogEntry const&)> && slot);

    void RemoveUnusedChannel();

    LogLevel GetLevel() const;

    void SetLevel(LogLevel verbosity);

    LogLevel GetLevel(std::string const& channelName) const;

    void SetLevel(std::string const& channelName, LogLevel verbosity);

    void Log(std::string const& message, LogLevel verbosity = LogLevel::Verbose);

    void Log(LogEntry const& entry);

    LogStream Stream(LogLevel verbosity = LogLevel::Verbose);

    LogStream Stream(std::string const& channelName, LogLevel verbosity = LogLevel::Verbose);

private:
    LogChannel defaultChannel;
    struct ChannelTuple {
        LogChannel channel;
        ScopedConnection connection;
    };
    std::vector<ChannelTuple> channels;
    std::recursive_mutex channelsProtection;
};
//-----------------------------------------------------------------------
Connection Logger::Connect(std::function<void(LogEntry const&)> const& slot)
{
    return defaultChannel.Connect(slot);
}
//-----------------------------------------------------------------------
Connection Logger::Connect(std::function<void(LogEntry const&)> && slot)
{
    return defaultChannel.Connect(std::move(slot));
}
//-----------------------------------------------------------------------
template <typename T>
static auto FindChannnel(std::string const& channelName, T & channels) ->decltype(channels.begin())
{
    return std::find_if(std::begin(channels), std::end(channels),
        [&channelName](decltype(channels.front()) const& tuple) {
            return channelName == tuple.channel.Name();
        });
}
//-----------------------------------------------------------------------
Connection Logger::Connect(std::string const& channelName, std::function<void(LogEntry const&)> const& slot)
{
    auto iter = FindChannnel(channelName, channels);
    if (std::end(channels) == iter)
    {
        std::lock_guard<std::recursive_mutex> lock(channelsProtection);

        ChannelTuple tuple;
        tuple.channel = LogChannel{channelName};
        channels.push_back(std::move(tuple));
        iter = std::prev(channels.end());
    }

    POMDOG_ASSERT(iter != std::end(channels));
    POMDOG_ASSERT(channelName == iter->channel.Name());

    auto & channel = iter->channel;
    auto & connection = iter->connection;

    connection = channel.Connect([this](LogEntry const& entry) {
        defaultChannel.Log(entry);
    });
    return channel.Connect(slot);
}
//-----------------------------------------------------------------------
Connection Logger::Connect(std::string const& channelName, std::function<void(LogEntry const&)> && slot)
{
    auto iter = FindChannnel(channelName, channels);
    if (std::end(channels) == iter)
    {
        std::lock_guard<std::recursive_mutex> lock(channelsProtection);

        ChannelTuple tuple;
        tuple.channel = LogChannel{channelName};
        channels.push_back(std::move(tuple));
        iter = std::prev(channels.end());
    }

    POMDOG_ASSERT(iter != std::end(channels));
    POMDOG_ASSERT(channelName == iter->channel.Name());

    auto & channel = iter->channel;
    auto & connection = iter->connection;

    connection = channel.Connect([this](LogEntry const& entry) {
        defaultChannel.Log(entry);
    });
    return channel.Connect(std::move(slot));
}
//-----------------------------------------------------------------------
void Logger::RemoveUnusedChannel()
{
    std::lock_guard<std::recursive_mutex> lock(channelsProtection);

    channels.erase(std::remove_if(std::begin(channels), std::end(channels),
        [](ChannelTuple const& tuple) {
            return tuple.channel.ConnectionCount() <= 0;
        }), std::end(channels));
}
//-----------------------------------------------------------------------
LogLevel Logger::GetLevel() const
{
    return defaultChannel.Level();
}
//-----------------------------------------------------------------------
void Logger::SetLevel(LogLevel level)
{
    defaultChannel.Level(level);
}
//-----------------------------------------------------------------------
LogLevel Logger::GetLevel(std::string const& channelName) const
{
    auto iter = FindChannnel(channelName, channels);
    if (iter != std::end(channels)) {
        return iter->channel.Level();
    }
    return defaultChannel.Level();
}
//-----------------------------------------------------------------------
void Logger::SetLevel(std::string const& channelName, LogLevel level)
{
    auto iter = FindChannnel(channelName, channels);
    if (iter != std::end(channels)) {
        iter->channel.Level(level);
    }
}
//-----------------------------------------------------------------------
void Logger::Log(std::string const& message, LogLevel verbosity)
{
    defaultChannel.Log(message, verbosity);
}
//-----------------------------------------------------------------------
void Logger::Log(LogEntry const& entry)
{
    auto iter = FindChannnel(entry.Tag, channels);
    if (std::end(channels) != iter) {
        iter->channel.Log(entry);
    }
    else {
        defaultChannel.Log(entry);
    }
}
//-----------------------------------------------------------------------
LogStream Logger::Stream(LogLevel verbosity)
{
    return LogStream(defaultChannel, verbosity);
}
//-----------------------------------------------------------------------
LogStream Logger::Stream(std::string const& channelName, LogLevel verbosity)
{
    auto iter = FindChannnel(channelName, channels);

    if (std::end(channels) != iter) {
        return LogStream(iter->channel, verbosity);
    }

    return LogStream(defaultChannel, channelName, verbosity);
}
//-----------------------------------------------------------------------
static Logger & GetLoggerInstance()
{
    static Logger logger;
    return logger;
}

}// unnamed namespace
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Log class
#endif
//-----------------------------------------------------------------------
Connection Log::Connect(std::function<void(LogEntry const&)> const& slot)
{
    auto & logger = GetLoggerInstance();
    return logger.Connect(slot);
}
//-----------------------------------------------------------------------
Connection Log::Connect(std::function<void(LogEntry const&)> && slot)
{
    auto & logger = GetLoggerInstance();
    return logger.Connect(std::move(slot));
}
//-----------------------------------------------------------------------
Connection Log::Connect(std::string const& channelName, std::function<void(LogEntry const&)> const& slot)
{
    auto & logger = GetLoggerInstance();
    return logger.Connect(channelName, slot);
}
//-----------------------------------------------------------------------
Connection Log::Connect(std::string const& channelName, std::function<void(LogEntry const&)> && slot)
{
    auto & logger = GetLoggerInstance();
    return logger.Connect(channelName, std::move(slot));
}
//-----------------------------------------------------------------------
LogStream Log::Stream(LogLevel verbosity)
{
    auto & logger = GetLoggerInstance();
    return logger.Stream(verbosity);
}
//-----------------------------------------------------------------------
LogStream Log::Stream(std::string const& channelName, LogLevel verbosity)
{
    auto & logger = GetLoggerInstance();
    return logger.Stream(channelName, verbosity);
}
//-----------------------------------------------------------------------
LogLevel Log::GetLevel()
{
    auto & logger = GetLoggerInstance();
    return logger.GetLevel();
}
//-----------------------------------------------------------------------
void Log::SetLevel(LogLevel verbosity)
{
    auto & logger = GetLoggerInstance();
    return logger.SetLevel(verbosity);
}
//-----------------------------------------------------------------------
LogLevel Log::GetLevel(std::string const& channelName)
{
    auto & logger = GetLoggerInstance();
    return logger.GetLevel(channelName);
}
//-----------------------------------------------------------------------
void Log::SetLevel(std::string const& channelName, LogLevel verbosity)
{
    auto & logger = GetLoggerInstance();
    return logger.SetLevel(channelName, verbosity);
}
//-----------------------------------------------------------------------
void Log::Critical(std::string const& channel, std::string const& message)
{
    auto & logger = GetLoggerInstance();
    logger.Log(LogEntry{message, channel, LogLevel::Critical});
}
//-----------------------------------------------------------------------
void Log::Warning(std::string const& channel, std::string const& message)
{
    auto & logger = GetLoggerInstance();
    logger.Log(LogEntry{message, channel, LogLevel::Warning});
}
//-----------------------------------------------------------------------
void Log::Info(std::string const& message)
{
    auto & logger = GetLoggerInstance();
    logger.Log(message, LogLevel::Info);
}
//-----------------------------------------------------------------------
void Log::Info(std::string const& channel, std::string const& message)
{
    auto & logger = GetLoggerInstance();
    logger.Log(LogEntry{message, channel, LogLevel::Info});
}
//-----------------------------------------------------------------------
void Log::Verbose(std::string const& message)
{
    auto & logger = GetLoggerInstance();
    logger.Log(message, LogLevel::Verbose);
}
//-----------------------------------------------------------------------
void Log::Verbose(std::string const& channel, std::string const& message)
{
    auto & logger = GetLoggerInstance();
    logger.Log(LogEntry{message, channel, LogLevel::Verbose});
}
//-----------------------------------------------------------------------
void Log::Internal(std::string const& message)
{
    auto & logger = GetLoggerInstance();
    logger.Log(message, LogLevel::Internal);
}
//-----------------------------------------------------------------------
void Log::Internal(std::string const& channel, std::string const& message)
{
    auto & logger = GetLoggerInstance();
    logger.Log(LogEntry{message, channel, LogLevel::Internal});
}
//-----------------------------------------------------------------------
}// namespace Pomdog
