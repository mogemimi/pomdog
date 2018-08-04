// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Logging/LogLevel.hpp"
#include "Pomdog/Signals/Signal.hpp"
#include <functional>
#include <string>

namespace Pomdog {

class LogEntry;

class POMDOG_EXPORT LogChannel {
public:
    explicit LogChannel(const std::string& name);

    LogChannel() = default;
    LogChannel(const LogChannel&) = delete;
    LogChannel(LogChannel &&) = default;
    LogChannel & operator=(const LogChannel&) = delete;
    LogChannel & operator=(LogChannel &&) = default;

    void Log(const std::string& message, LogLevel verbosity);

    void Log(const LogEntry& logEntry);

    Connection Connect(const std::function<void(const LogEntry&)>& slot);

    Connection Connect(std::function<void(const LogEntry&)> && slot);

    const std::string& Name() const;

    LogLevel Level() const;

    void Level(LogLevel level);

    std::size_t ConnectionCount() const;

private:
    Signal<void(const LogEntry&)> signal;
    std::string name;
    LogLevel level;
};

} // namespace Pomdog
