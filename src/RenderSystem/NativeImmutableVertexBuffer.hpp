//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVEIMMUTABLEVERTEXBUFFER_040F91F1_FC4C_49C9_A07F_E3D78229BC8A_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVEIMMUTABLEVERTEXBUFFER_040F91F1_FC4C_49C9_A07F_E3D78229BC8A_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "NativeVertexBuffer.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeImmutableVertexBuffer: public NativeVertexBuffer
{
public:
	virtual ~NativeImmutableVertexBuffer() override = default;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVEIMMUTABLEVERTEXBUFFER_040F91F1_FC4C_49C9_A07F_E3D78229BC8A_HPP)
