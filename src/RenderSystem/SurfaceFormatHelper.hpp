// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SURFACEFORMATHELPER_BDAD06C9_A6EC_4A3C_A1AA_A82A6C9B0B1F_HPP
#define POMDOG_SURFACEFORMATHELPER_BDAD06C9_A6EC_4A3C_A1AA_A82A6C9B0B1F_HPP

#include <cstdint>

namespace Pomdog {

enum class SurfaceFormat : std::uint8_t;

namespace Detail {
namespace RenderSystem {

struct SurfaceFormatHelper final {
    static int ToBytesPerBlock(SurfaceFormat format);
};

}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_SURFACEFORMATHELPER_BDAD06C9_A6EC_4A3C_A1AA_A82A6C9B0B1F_HPP)
