// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/time_point.hpp"

namespace pomdog::detail::emscripten {

class TimeSourceEmscripten final {
public:
    TimePoint Now() const;
};

} // namespace pomdog::detail::emscripten
