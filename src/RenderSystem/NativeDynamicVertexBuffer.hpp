//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVEDYNAMICVERTEXBUFFER_B12C6A33_1941_4923_AFDB_7FE1C5FBCA32_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVEDYNAMICVERTEXBUFFER_B12C6A33_1941_4923_AFDB_7FE1C5FBCA32_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "NativeVertexBuffer.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeDynamicVertexBuffer: public NativeVertexBuffer
{
public:
	virtual ~NativeDynamicVertexBuffer() override = default;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVEDYNAMICVERTEXBUFFER_B12C6A33_1941_4923_AFDB_7FE1C5FBCA32_HPP)
