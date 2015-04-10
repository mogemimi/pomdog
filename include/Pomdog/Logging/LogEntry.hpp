// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_LOGENTRY_F0B169C2_HPP
#define POMDOG_LOGENTRY_F0B169C2_HPP

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

#endif // POMDOG_LOGENTRY_F0B169C2_HPP
