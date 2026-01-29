// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#if __has_include(<stdfloat>)
#include <stdfloat>
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

#if __cplusplus >= 202302L && __has_include(<stdfloat>) && defined(__STDCPP_FLOAT32_T__)
using f32 = std::float32_t;
#else
using f32 = float;
#endif

#if __cplusplus >= 202302L && __has_include(<stdfloat>) && defined(__STDCPP_FLOAT64_T__)
using f64 = std::float64_t;
#else
using f64 = double;
#endif

} // namespace pomdog
