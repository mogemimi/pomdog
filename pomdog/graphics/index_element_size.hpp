// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

enum class IndexElementSize : std::uint8_t {
    /// A 16-bit unsigned integer used as a the size of a primitive index.
    SixteenBits,

    /// A 32-bit unsigned nteger used as a the size of a primitive index.
    ThirtyTwoBits,
};

} // namespace Pomdog
