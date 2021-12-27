// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/logging/log_level.h"
#include "pomdog/signals/signal.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class LogEntry;

class POMDOG_EXPORT LogChannel final {
public:
    explicit LogChannel(const std::string& name);

    LogChannel() = default;
    LogChannel(const LogChannel&) = delete;
    LogChannel(LogChannel&&) = default;
    LogChannel& operator=(const LogChannel&) = delete;
    LogChannel& operator=(LogChannel&&) = default;

    void Log(const std::string& message, LogLevel verbosity);

    void Log(const LogEntry& logEntry);

    [[nodiscard]] Connection Connect(const std::function<void(const LogEntry&)>& slot);

    [[nodiscard]] Connection Connect(std::function<void(const LogEntry&)>&& slot);

    [[nodiscard]] const std::string& GetName() const;

    [[nodiscard]] LogLevel GetLevel() const noexcept;

    void SetLevel(LogLevel level) noexcept;

    [[nodiscard]] std::size_t GetConnectionCount() const;

private:
    Signal<void(const LogEntry&)> signal;
    std::string name;
    LogLevel level;
};

} // namespace pomdog
