//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_VERTEXBUFFER_3DDFD5CB_025A_40AE_9438_85D3064CE7D9_HPP
#define POMDOG_VERTEXBUFFER_3DDFD5CB_025A_40AE_9438_85D3064CE7D9_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/Export.hpp"
#include "detail/ForwardDeclarations.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeVertexBuffer;

}// namespace RenderSystem
}// namespace Details

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief 頂点バッファです。
class POMDOG_EXPORT VertexBuffer
{
public:
	virtual	~VertexBuffer() = default;

	///@~Japanese
	/// @brief 頂点データの定義を取得します。
	virtual VertexDeclaration const& GetVertexDeclaration() const = 0;

	///@~Japanese
	/// @brief 頂点の数を取得します。
	virtual std::uint32_t GetVertexCount() const = 0;

	///@~Japanese
	/// @brief バッファの使用方法を取得します。
	virtual BufferUsage GetBufferUsage() const = 0;
	
public:
	virtual Details::RenderSystem::NativeVertexBuffer* GetNativeVertexBuffer() = 0;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_VERTEXBUFFER_3DDFD5CB_025A_40AE_9438_85D3064CE7D9_HPP)
