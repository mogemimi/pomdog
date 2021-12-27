// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/chrono/time_point.hpp"
#include "pomdog/chrono/time_source.hpp"

namespace pomdog::detail::emscripten {

class TimeSourceEmscripten final : public TimeSource {
public:
    [[nodiscard]] TimePoint Now() const noexcept override;
};

} // namespace pomdog::detail::emscripten
