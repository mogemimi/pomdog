//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DXGIFORMATHELPER_9FB57D4A_D20F_4F3C_8A2F_DA0696D56CD1_HPP
#define POMDOG_DXGIFORMATHELPER_9FB57D4A_D20F_4F3C_8A2F_DA0696D56CD1_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "PrerequisitesDirect3D11.hpp"
#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace Direct3D11 {

class DXGIFormatHelper final {
public:
	static DXGI_FORMAT ToDXGIFormat(DepthFormat format);
	static DXGI_FORMAT ToDXGIFormat(SurfaceFormat format);
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_DXGIFORMATHELPER_9FB57D4A_D20F_4F3C_8A2F_DA0696D56CD1_HPP)
