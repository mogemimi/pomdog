// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/time_point.hpp"

namespace Pomdog::Detail::Linux {

class TimeSourceLinux final {
public:
    TimeSourceLinux() = default;

    TimePoint Now() const;
};

} // namespace Pomdog::Detail::Linux
