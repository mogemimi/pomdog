// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_LOG_B5157F87_HPP
#define POMDOG_LOG_B5157F87_HPP

#include "LogLevel.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <string>
#include <functional>

namespace Pomdog {

class Connection;
class LogEntry;

class POMDOG_EXPORT Log {
public:
    static Connection Connect(std::function<void(LogEntry const&)> const& slot);

    static Connection Connect(std::function<void(LogEntry const&)> && slot);

    static Connection Connect(std::string const& channelName,
        std::function<void(LogEntry const&)> const& slot);

    static Connection Connect(std::string const& channelName,
        std::function<void(LogEntry const&)> && slot);

    static LogLevel GetLevel();

    static void SetLevel(LogLevel verbosity);

    static LogLevel GetLevel(std::string const& channelName);

    static void SetLevel(std::string const& channelName, LogLevel verbosity);

    static void Critical(std::string const& channel, std::string const& message);

    static void Warning(std::string const& channel, std::string const& message);

    static void Info(std::string const& message);

    static void Info(std::string const& channel, std::string const& message);

    static void Verbose(std::string const& message);

    static void Verbose(std::string const& channel, std::string const& message);

    static void Internal(std::string const& message);

    static void Internal(std::string const& channel, std::string const& message);
};

} // namespace Pomdog

#endif // POMDOG_LOG_B5157F87_HPP
