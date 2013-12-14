//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_NATIVEINDEXBUFFER_C6FFE20E_160B_4B28_BD28_4F4A509BEC47_HPP
#define POMDOG_NATIVEINDEXBUFFER_C6FFE20E_160B_4B28_BD28_4F4A509BEC47_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeIndexBuffer
{
public:
	virtual ~NativeIndexBuffer() = default;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_NATIVEINDEXBUFFER_C6FFE20E_160B_4B28_BD28_4F4A509BEC47_HPP)
