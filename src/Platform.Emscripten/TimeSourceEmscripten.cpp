// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "TimeSourceEmscripten.hpp"
#include <emscripten.h>

namespace Pomdog::Detail::Emscripten {

TimePoint TimeSourceEmscripten::Now() const
{
    const auto now = ::emscripten_get_now();
    return TimePoint{Duration{static_cast<double>(now)}};
}

} // namespace Pomdog::Detail::Emscripten
