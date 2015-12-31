// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class BufferUsage : std::uint8_t {
    Immutable,
    Dynamic,
};

} // namespace Pomdog
