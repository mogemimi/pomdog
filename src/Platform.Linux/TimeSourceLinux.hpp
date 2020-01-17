// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/TimePoint.hpp"

namespace Pomdog::Detail::Linux {

class TimeSourceLinux final {
public:
    TimeSourceLinux() = default;

    TimePoint Now() const;
};

} // namespace Pomdog::Detail::Linux
