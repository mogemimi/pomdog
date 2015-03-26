// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_LOGCHANNEL_4C388D05_HPP
#define POMDOG_LOGCHANNEL_4C388D05_HPP

#include "LogLevel.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Event/Signal.hpp"
#include <functional>
#include <string>

namespace Pomdog {

class LogEntry;

class POMDOG_EXPORT LogChannel {
public:
    explicit LogChannel(std::string const& name);

    LogChannel() = default;
    LogChannel(LogChannel const&) = delete;
    LogChannel(LogChannel &&) = default;
    LogChannel & operator=(LogChannel const&) = delete;
    LogChannel & operator=(LogChannel &&) = default;

    void Log(std::string const& message, LogLevel verbosity);

    void Log(LogEntry const& logEntry);

    EventConnection Connect(std::function<void(LogEntry const&)> const& slot);

    EventConnection Connect(std::function<void(LogEntry const&)> && slot);

    std::string const& Name() const;

    LogLevel Level() const;

    void Level(LogLevel level);

    std::size_t ConnectionCount() const;

private:
    Signal<void(LogEntry const&)> signal;
    std::string name;
    LogLevel level;
};

}// namespace Pomdog

#endif // POMDOG_LOGCHANNEL_4C388D05_HPP
