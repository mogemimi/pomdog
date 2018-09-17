// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Logging/LogChannel.hpp"
#include "Pomdog/Logging/LogEntry.hpp"
#include "Pomdog/Signals/Connection.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {

LogChannel::LogChannel(const std::string& channnelName)
    : name(channnelName)
    , level(LogLevel::Verbose)
{
}

void LogChannel::Log(const std::string& message, LogLevel verbosity)
{
    if (verbosity <= this->level) {
        signal(LogEntry{message, name, verbosity});
    }
}

void LogChannel::Log(const LogEntry& logEntry)
{
    if (logEntry.Verbosity <= this->level) {
        signal(logEntry);
    }
}

Connection LogChannel::Connect(const std::function<void(const LogEntry&)>& slot)
{
    POMDOG_ASSERT(slot);
    return signal.Connect(slot);
}

Connection LogChannel::Connect(std::function<void(const LogEntry&)> && slot)
{
    POMDOG_ASSERT(slot);
    return signal.Connect(std::move(slot));
}

const std::string& LogChannel::GetName() const
{
    return name;
}

LogLevel LogChannel::GetLevel() const noexcept
{
    return this->level;
}

void LogChannel::SetLevel(LogLevel levelIn) noexcept
{
    this->level = levelIn;
}

std::size_t LogChannel::GetConnectionCount() const
{
    return signal.InvocationCount();
}

} // namespace Pomdog
