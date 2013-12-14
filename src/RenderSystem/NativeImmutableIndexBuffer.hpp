//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVEIMMUTABLEINDEXBUFFER_356526A0_CB76_414B_8C95_9BB9931B6034_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVEIMMUTABLEINDEXBUFFER_356526A0_CB76_414B_8C95_9BB9931B6034_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "NativeIndexBuffer.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeImmutableIndexBuffer: public NativeIndexBuffer
{
public:
	virtual ~NativeImmutableIndexBuffer() override = default;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVEIMMUTABLEINDEXBUFFER_356526A0_CB76_414B_8C95_9BB9931B6034_HPP)
