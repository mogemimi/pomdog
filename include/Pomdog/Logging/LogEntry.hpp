// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_LOGENTRY_F0B169C2_B3E2_4144_939B_F1FBE89BF3AC_HPP
#define POMDOG_LOGENTRY_F0B169C2_B3E2_4144_939B_F1FBE89BF3AC_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "LogLevel.hpp"
#include <string>

namespace Pomdog {

class LogEntry {
public:
    std::string Message;

    std::string Tag;

    LogLevel Verbosity;
};

}// namespace Pomdog

#endif // !defined(POMDOG_LOGENTRY_F0B169C2_B3E2_4144_939B_F1FBE89BF3AC_HPP)
