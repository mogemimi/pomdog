// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TEXTUREHELPER_FDC1E68E_244E_4210_AA2A_9DE6C8AD9BC9_HPP
#define POMDOG_TEXTUREHELPER_FDC1E68E_244E_4210_AA2A_9DE6C8AD9BC9_HPP

#include <cstdint>

namespace Pomdog {
namespace Detail {

struct TextureHelper final {
    static std::int32_t ComputeMipmapLevelCount(
        std::int32_t width, std::int32_t height);
};

}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_TEXTUREHELPER_FDC1E68E_244E_4210_AA2A_9DE6C8AD9BC9_HPP)
