// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TEXTUREADDRESSMODE_256FCFEC_HPP
#define POMDOG_TEXTUREADDRESSMODE_256FCFEC_HPP

#include <cstdint>

namespace Pomdog {

enum class TextureAddressMode: std::uint8_t {
    Wrap,

    Mirror,

    Clamp,
};

}// namespace Pomdog

#endif // POMDOG_TEXTUREADDRESSMODE_256FCFEC_HPP
