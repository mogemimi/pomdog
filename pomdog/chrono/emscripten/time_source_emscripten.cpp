// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/chrono/emscripten/time_source_emscripten.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten.h>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::emscripten {

TimePoint TimeSourceEmscripten::now() const noexcept
{
    const auto now = ::emscripten_get_now();
    static_assert(std::is_same_v<std::remove_const_t<decltype(now)>, double>);
    return TimePoint{Duration{now * 0.001}};
}

} // namespace pomdog::detail::emscripten
