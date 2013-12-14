//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_IMMUTABLEVERTEXBUFFER_B469F4AA_82E5_4EA5_B570_01F911130E40_HPP
#define POMDOG_IMMUTABLEVERTEXBUFFER_B469F4AA_82E5_4EA5_B570_01F911130E40_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include "VertexBuffer.hpp"
#include "BufferUsage.hpp"
#include "VertexDeclaration.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeImmutableVertexBuffer;

}// namespace RenderSystem
}// namespace Details

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief 静的な頂点バッファです。
class ImmutableVertexBuffer: public VertexBuffer
{
public:
	ImmutableVertexBuffer() = delete;
	ImmutableVertexBuffer(ImmutableVertexBuffer const&) = delete;
	ImmutableVertexBuffer(ImmutableVertexBuffer &&) = default;

	~ImmutableVertexBuffer() override;

	ImmutableVertexBuffer & operator=(ImmutableVertexBuffer const&) = delete;
	ImmutableVertexBuffer & operator=(ImmutableVertexBuffer &&) = default;

	///@~Japanese
	/// @brief 頂点データの定義を取得します。
	VertexDeclaration const& GetVertexDeclaration() const;

	///@~Japanese
	/// @brief バッファの使用方法を取得します。
	BufferUsage GetBufferUsage() const;

	///@~Japanese
	/// @brief 頂点の数を取得します。
	std::size_t GetVertexCount() const;
	
public:
	Details::RenderSystem::NativeVertexBuffer* GetNativeVertexBuffer();

private:
	std::unique_ptr<Details::RenderSystem::NativeImmutableVertexBuffer> nativeVertexBuffer;
	VertexDeclaration vertexDeclaration;
	std::size_t vertexCount;
	BufferUsage bufferUsage;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_IMMUTABLEVERTEXBUFFER_B469F4AA_82E5_4EA5_B570_01F911130E40_HPP)
