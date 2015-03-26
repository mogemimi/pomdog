// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SPRITESORTMODE_670C0FC1_7802_46A0_BF22_C8C399025B2B_HPP
#define POMDOG_SPRITESORTMODE_670C0FC1_7802_46A0_BF22_C8C399025B2B_HPP

#include <cstdint>

namespace Pomdog {

enum class SpriteSortMode: std::uint8_t {
    Deferred,

    BackToFront,

    FrontToBack,
};

}// namespace Pomdog

#endif // !defined(POMDOG_SPRITESORTMODE_670C0FC1_7802_46A0_BF22_C8C399025B2B_HPP)
