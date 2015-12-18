// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_SPRITESORTMODE_670C0FC1_HPP
#define POMDOG_SPRITESORTMODE_670C0FC1_HPP

#include <cstdint>

namespace Pomdog {

enum class SpriteSortMode : std::uint8_t {
    Deferred,
    BackToFront,
    FrontToBack,
};

} // namespace Pomdog

#endif // POMDOG_SPRITESORTMODE_670C0FC1_HPP
