//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVEDYNAMICINDEXBUFFER_81B1942E_E9A2_4B54_B1BE_3B5A434C8B5C_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVEDYNAMICINDEXBUFFER_81B1942E_E9A2_4B54_B1BE_3B5A434C8B5C_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "NativeIndexBuffer.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeDynamicIndexBuffer: public NativeIndexBuffer
{
public:
	virtual ~NativeDynamicIndexBuffer() override = default;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVEDYNAMICINDEXBUFFER_81B1942E_E9A2_4B54_B1BE_3B5A434C8B5C_HPP)
