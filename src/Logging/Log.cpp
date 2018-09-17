// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Logging/LogChannel.hpp"
#include "Pomdog/Logging/LogEntry.hpp"
#include "Pomdog/Signals/Connection.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include <algorithm>
#include <memory>
#include <mutex>
#include <vector>

namespace Pomdog {
namespace {

class Logger final {
public:
    Connection Connect(const std::function<void(const LogEntry&)>& slot);

    Connection Connect(std::function<void(const LogEntry&)> && slot);

    Connection Connect(
        const std::string& channelName,
        const std::function<void(const LogEntry&)>& slot);

    Connection Connect(
        const std::string& channelName,
        std::function<void(const LogEntry&)> && slot);

    LogLevel GetLevel() const;

    void SetLevel(LogLevel verbosity);

    LogLevel GetLevel(const std::string& channelName) const;

    void SetLevel(const std::string& channelName, LogLevel verbosity);

    void Log(const std::string& message, LogLevel verbosity);

    void Log(const LogEntry& entry);

private:
    LogChannel defaultChannel;
    struct ChannelTuple {
        LogChannel channel;
        ScopedConnection connection;
    };
    std::vector<ChannelTuple> channels;
    std::recursive_mutex channelsProtection;
};

Connection Logger::Connect(const std::function<void(const LogEntry&)>& slot)
{
    return defaultChannel.Connect(slot);
}

Connection Logger::Connect(std::function<void(const LogEntry&)> && slot)
{
    return defaultChannel.Connect(std::move(slot));
}

template <typename T>
auto FindChannnel(const std::string& channelName, T & channels)
    ->decltype(channels.begin())
{
    return std::find_if(std::begin(channels), std::end(channels),
        [&channelName](const decltype(channels.front())& tuple) {
            return channelName == tuple.channel.GetName();
        });
}

Connection Logger::Connect(
    const std::string& channelName,
    const std::function<void(const LogEntry&)>& slot)
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
    POMDOG_ASSERT(channelName == iter->channel.GetName());

    auto & channel = iter->channel;
    auto & connection = iter->connection;

    connection = channel.Connect([this](const LogEntry& entry) {
        defaultChannel.Log(entry);
    });
    return channel.Connect(slot);
}

Connection Logger::Connect(
    const std::string& channelName,
    std::function<void(const LogEntry&)> && slot)
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
    POMDOG_ASSERT(channelName == iter->channel.GetName());

    auto & channel = iter->channel;
    auto & connection = iter->connection;

    connection = channel.Connect([this](const LogEntry& entry) {
        defaultChannel.Log(entry);
    });
    return channel.Connect(std::move(slot));
}

LogLevel Logger::GetLevel() const
{
    return defaultChannel.GetLevel();
}

void Logger::SetLevel(LogLevel level)
{
    defaultChannel.SetLevel(level);
}

LogLevel Logger::GetLevel(const std::string& channelName) const
{
    auto iter = FindChannnel(channelName, channels);
    if (iter != std::end(channels)) {
        return iter->channel.GetLevel();
    }
    return defaultChannel.GetLevel();
}

void Logger::SetLevel(const std::string& channelName, LogLevel level)
{
    auto iter = FindChannnel(channelName, channels);
    if (iter != std::end(channels)) {
        iter->channel.SetLevel(level);
    }
}

void Logger::Log(const std::string& message, LogLevel verbosity)
{
    defaultChannel.Log(message, verbosity);
}

void Logger::Log(const LogEntry& entry)
{
    auto iter = FindChannnel(entry.Tag, channels);
    if (std::end(channels) != iter) {
        iter->channel.Log(entry);
    }
    else {
        defaultChannel.Log(entry);
    }
}

Logger & GetLoggerInstance()
{
    static Logger logger;
    return logger;
}

} // unnamed namespace

// MARK: - Log class

Connection Log::Connect(const std::function<void(const LogEntry&)>& slot)
{
    auto & logger = GetLoggerInstance();
    return logger.Connect(slot);
}

Connection Log::Connect(std::function<void(const LogEntry&)> && slot)
{
    auto & logger = GetLoggerInstance();
    return logger.Connect(std::move(slot));
}

Connection Log::Connect(const std::string& channelName,
    const std::function<void(const LogEntry&)>& slot)
{
    auto & logger = GetLoggerInstance();
    return logger.Connect(channelName, slot);
}

Connection Log::Connect(const std::string& channelName,
    std::function<void(const LogEntry&)> && slot)
{
    auto & logger = GetLoggerInstance();
    return logger.Connect(channelName, std::move(slot));
}

LogLevel Log::GetLevel()
{
    auto & logger = GetLoggerInstance();
    return logger.GetLevel();
}

void Log::SetLevel(LogLevel verbosity)
{
    auto & logger = GetLoggerInstance();
    return logger.SetLevel(verbosity);
}

LogLevel Log::GetLevel(const std::string& channelName)
{
    auto & logger = GetLoggerInstance();
    return logger.GetLevel(channelName);
}

void Log::SetLevel(const std::string& channelName, LogLevel verbosity)
{
    auto & logger = GetLoggerInstance();
    return logger.SetLevel(channelName, verbosity);
}

void Log::Critical(const std::string& channelName, const std::string& message)
{
    auto & logger = GetLoggerInstance();
    logger.Log(LogEntry{message, channelName, LogLevel::Critical});
}

void Log::Warning(const std::string& channelName, const std::string& message)
{
    auto & logger = GetLoggerInstance();
    logger.Log(LogEntry{message, channelName, LogLevel::Warning});
}

void Log::Info(const std::string& message)
{
    auto & logger = GetLoggerInstance();
    logger.Log(message, LogLevel::Info);
}

void Log::Info(const std::string& channelName, const std::string& message)
{
    auto & logger = GetLoggerInstance();
    logger.Log(LogEntry{message, channelName, LogLevel::Info});
}

void Log::Verbose(const std::string& message)
{
    auto & logger = GetLoggerInstance();
    logger.Log(message, LogLevel::Verbose);
}

void Log::Verbose(const std::string& channelName, const std::string& message)
{
    auto & logger = GetLoggerInstance();
    logger.Log(LogEntry{message, channelName, LogLevel::Verbose});
}

void Log::Internal(const std::string& message)
{
    auto & logger = GetLoggerInstance();
    logger.Log(message, LogLevel::Internal);
}

void Log::Internal(const std::string& channelName, const std::string& message)
{
    auto & logger = GetLoggerInstance();
    logger.Log(LogEntry{message, channelName, LogLevel::Internal});
}

} // namespace Pomdog
