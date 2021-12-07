// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/time_point.hpp"

namespace Pomdog::Detail::Emscripten {

class TimeSourceEmscripten final {
public:
    TimePoint Now() const;
};

} // namespace Pomdog::Detail::Emscripten
