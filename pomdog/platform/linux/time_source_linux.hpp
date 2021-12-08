// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/time_point.hpp"

namespace pomdog::detail::linux {

class TimeSourceLinux final {
public:
    TimeSourceLinux() = default;

    TimePoint Now() const;
};

} // namespace pomdog::detail::linux
