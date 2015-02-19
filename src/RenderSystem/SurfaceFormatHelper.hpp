//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SURFACEFORMATHELPER_BDAD06C9_A6EC_4A3C_A1AA_A82A6C9B0B1F_HPP
#define POMDOG_SURFACEFORMATHELPER_BDAD06C9_A6EC_4A3C_A1AA_A82A6C9B0B1F_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

enum class SurfaceFormat : std::uint8_t;

namespace Details {
namespace RenderSystem {

struct SurfaceFormatHelper final {
	static int ToBytesPerBlock(SurfaceFormat format);
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SURFACEFORMATHELPER_BDAD06C9_A6EC_4A3C_A1AA_A82A6C9B0B1F_HPP)
