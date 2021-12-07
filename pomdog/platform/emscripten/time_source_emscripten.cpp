// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/platform/emscripten/time_source_emscripten.hpp"
#include <emscripten.h>
#include <type_traits>

namespace Pomdog::Detail::Emscripten {

TimePoint TimeSourceEmscripten::Now() const
{
    const auto now = ::emscripten_get_now();
    static_assert(std::is_same_v<std::remove_const_t<decltype(now)>, double>);
    return TimePoint{Duration{now * 0.001}};
}

} // namespace Pomdog::Detail::Emscripten
