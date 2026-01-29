// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

static_assert(sizeof(i8) == 1);
static_assert(sizeof(i16) == 2);
static_assert(sizeof(i32) == 4);
static_assert(sizeof(i64) == 8);

static_assert(sizeof(u8) == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(u64) == 8);

static_assert(sizeof(f32) == 4);
static_assert(sizeof(f64) == 8);

static_assert(std::is_fundamental_v<i8>);
static_assert(std::is_fundamental_v<i16>);
static_assert(std::is_fundamental_v<i32>);
static_assert(std::is_fundamental_v<i64>);

static_assert(std::is_fundamental_v<u8>);
static_assert(std::is_fundamental_v<u16>);
static_assert(std::is_fundamental_v<u32>);
static_assert(std::is_fundamental_v<u64>);

static_assert(std::is_fundamental_v<f32>);
static_assert(std::is_fundamental_v<f64>);

static_assert(std::is_signed_v<i8>);
static_assert(std::is_signed_v<i16>);
static_assert(std::is_signed_v<i32>);
static_assert(std::is_signed_v<i64>);

static_assert(std::is_unsigned_v<u8>);
static_assert(std::is_unsigned_v<u16>);
static_assert(std::is_unsigned_v<u32>);
static_assert(std::is_unsigned_v<u64>);

static_assert(std::is_floating_point_v<f32>);
static_assert(std::is_floating_point_v<f64>);

} // namespace pomdog
