// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "LogLevel.hpp"
#include <string>

namespace Pomdog {

class LogEntry {
public:
    std::string Message;
    std::string Tag;
    LogLevel Verbosity;
};

} // namespace Pomdog
