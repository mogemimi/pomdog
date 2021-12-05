// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/TimePoint.hpp"

namespace Pomdog::Detail::Emscripten {

class TimeSourceEmscripten final {
public:
    TimePoint Now() const;
};

} // namespace Pomdog::Detail::Emscripten
