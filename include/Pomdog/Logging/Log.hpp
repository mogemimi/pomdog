// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Logging/LogLevel.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <string>
#include <functional>

namespace Pomdog {

class Connection;
class LogEntry;

class POMDOG_EXPORT Log {
public:
    static Connection Connect(const std::function<void(const LogEntry&)>& slot);

    static Connection Connect(std::function<void(const LogEntry&)> && slot);

    static Connection Connect(
        const std::string& channelName,
        const std::function<void(const LogEntry&)>& slot);

    static Connection Connect(
        const std::string& channelName,
        std::function<void(const LogEntry&)> && slot);

    static LogLevel GetLevel();

    static void SetLevel(LogLevel verbosity);

    static LogLevel GetLevel(const std::string& channelName);

    static void SetLevel(const std::string& channelName, LogLevel verbosity);

    static void Critical(const std::string& channel, const std::string& message);

    static void Warning(const std::string& channel, const std::string& message);

    static void Info(const std::string& message);

    static void Info(const std::string& channel, const std::string& message);

    static void Verbose(const std::string& message);

    static void Verbose(const std::string& channel, const std::string& message);

    static void Internal(const std::string& message);

    static void Internal(const std::string& channel, const std::string& message);
};

} // namespace Pomdog
