// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Logging/LogLevel.hpp"
#include <functional>
#include <string>

namespace Pomdog {

class Connection;
class LogEntry;

class POMDOG_EXPORT Log final {
public:
    static Connection Connect(const std::function<void(const LogEntry&)>& slot);

    static Connection Connect(std::function<void(const LogEntry&)>&& slot);

    static Connection Connect(
        const std::string& channelName,
        const std::function<void(const LogEntry&)>& slot);

    static Connection Connect(
        const std::string& channelName,
        std::function<void(const LogEntry&)>&& slot);

    static LogLevel GetLevel();

    static void SetLevel(LogLevel verbosity);

    static LogLevel GetLevel(const std::string& channelName);

    static void SetLevel(const std::string& channelName, LogLevel verbosity);

    static void Critical(const std::string& channelName, const std::string& message);

    static void Warning(const std::string& channelName, const std::string& message);

    static void Info(const std::string& message);

    static void Info(const std::string& channelName, const std::string& message);

    static void Verbose(const std::string& message);

    static void Verbose(const std::string& channelName, const std::string& message);

    static void Internal(const std::string& message);

    static void Internal(const std::string& channelName, const std::string& message);
};

} // namespace Pomdog
