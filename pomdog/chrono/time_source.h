// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.hpp"
#include "pomdog/chrono/time_point.hpp"

namespace pomdog::detail {

class POMDOG_EXPORT TimeSource {
public:
    virtual ~TimeSource() noexcept;

    [[nodiscard]] virtual TimePoint Now() const noexcept = 0;
};

} // namespace pomdog::detail
