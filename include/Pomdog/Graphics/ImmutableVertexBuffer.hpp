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
#include "../Config/Export.hpp"
#include "detail/ForwardDeclarations.hpp"
#include "VertexBuffer.hpp"
#include "VertexDeclaration.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief 静的な頂点バッファです。
class POMDOG_EXPORT ImmutableVertexBuffer: public VertexBuffer
{
public:
	ImmutableVertexBuffer() = delete;
	ImmutableVertexBuffer(ImmutableVertexBuffer const&) = delete;
	ImmutableVertexBuffer(ImmutableVertexBuffer &&) = default;

	ImmutableVertexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
		VertexDeclaration const& vertexDeclaration, void const* vertices, std::uint32_t vertexCount);

	ImmutableVertexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
		VertexDeclaration && vertexDeclaration, void const* vertices, std::uint32_t vertexCount);

	~ImmutableVertexBuffer() override;

	ImmutableVertexBuffer & operator=(ImmutableVertexBuffer const&) = delete;
	ImmutableVertexBuffer & operator=(ImmutableVertexBuffer &&) = default;

	///@~Japanese
	/// @brief 頂点データの定義を取得します。
	VertexDeclaration const& GetVertexDeclaration() const override;

	///@~Japanese
	/// @brief バッファの使用方法を取得します。
	BufferUsage GetBufferUsage() const override;

	///@~Japanese
	/// @brief 頂点の数を取得します。
	std::uint32_t GetVertexCount() const override;
	
public:
	Details::RenderSystem::NativeVertexBuffer* GetNativeVertexBuffer() override;

private:
	VertexDeclaration vertexDeclaration;
	std::unique_ptr<Details::RenderSystem::NativeVertexBuffer> nativeVertexBuffer;
	std::uint32_t vertexCount;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_IMMUTABLEVERTEXBUFFER_B469F4AA_82E5_4EA5_B570_01F911130E40_HPP)
