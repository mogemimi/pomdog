//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_INDEXBUFFER_4119FC34_308A_4EF7_BC78_92FD2D6DBC43_HPP
#define POMDOG_INDEXBUFFER_4119FC34_308A_4EF7_BC78_92FD2D6DBC43_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>
#include <memory>
#include "../Config/Export.hpp"
#include "detail/ForwardDeclarations.hpp"
#include "BufferUsage.hpp"
#include "IndexElementSize.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeIndexBuffer;

}// namespace RenderSystem
}// namespace Details

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief インデックスバッファです。
class POMDOG_EXPORT IndexBuffer {
public:
	virtual	~IndexBuffer() = default;

	///@~Japanese
	/// @brief インデックスの個数を取得します。
	virtual std::uint32_t IndexCount() const = 0;

	///@~Japanese
	/// @brief インデックスの要素 1 つ分のサイズを取得します。
	virtual IndexElementSize ElementSize() const = 0;

	///@~Japanese
	/// @brief バッファの使用方法を取得します。
	virtual BufferUsage BufferUsage() const = 0;

public:
	virtual Details::RenderSystem::NativeIndexBuffer* NativeIndexBuffer() = 0;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_INDEXBUFFER_4119FC34_308A_4EF7_BC78_92FD2D6DBC43_HPP)
