// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_DXGIFORMATHELPER_9FB57D4A_D20F_4F3C_8A2F_DA0696D56CD1_HPP
#define POMDOG_DXGIFORMATHELPER_9FB57D4A_D20F_4F3C_8A2F_DA0696D56CD1_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "PrerequisitesDirect3D11.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {

struct DXGIFormatHelper final {
	static DXGI_FORMAT ToDXGIFormat(DepthFormat format);
	static DXGI_FORMAT ToDXGIFormat(SurfaceFormat format);
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_DXGIFORMATHELPER_9FB57D4A_D20F_4F3C_8A2F_DA0696D56CD1_HPP)
