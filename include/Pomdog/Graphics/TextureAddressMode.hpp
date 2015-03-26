// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TEXTUREADDRESSMODE_256FCFEC_6DE0_40B1_9326_E113017D0F57_HPP
#define POMDOG_TEXTUREADDRESSMODE_256FCFEC_6DE0_40B1_9326_E113017D0F57_HPP

#include <cstdint>

namespace Pomdog {

enum class TextureAddressMode: std::uint8_t {
    Wrap,

    Mirror,

    Clamp,
};

}// namespace Pomdog

#endif // !defined(POMDOG_TEXTUREADDRESSMODE_256FCFEC_6DE0_40B1_9326_E113017D0F57_HPP)
