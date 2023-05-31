// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/chrono/time_point.h"
#include "pomdog/chrono/time_source.h"

namespace pomdog::detail::emscripten {

class TimeSourceEmscripten final : public TimeSource {
public:
    [[nodiscard]] TimePoint
    now() const noexcept override;
};

} // namespace pomdog::detail::emscripten
