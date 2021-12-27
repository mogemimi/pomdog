// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/chrono/time_point.h"
#include "pomdog/chrono/time_source.h"

namespace pomdog::detail::linux {

class TimeSourceLinux final : public TimeSource {
public:
    [[nodiscard]] TimePoint Now() const noexcept override;
};

} // namespace pomdog::detail::linux
