// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class SpriteSortMode : std::uint8_t {
    Deferred,
    BackToFront,
    FrontToBack,
};

} // namespace Pomdog
