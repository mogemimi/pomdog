//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVEGRAPHICSCONTEXT_D292A72E_1944_423B_B25A_6A311774AC04_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVEGRAPHICSCONTEXT_D292A72E_1944_423B_B25A_6A311774AC04_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <vector>
#include <Pomdog/Utility/Noncopyable.hpp>
#include <Pomdog/Math/detail/ForwardDeclarations.hpp>
#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class GraphicsCapabilities;
class NativeEffectPass;

class NativeGraphicsContext: Noncopyable
{
public:
	virtual ~NativeGraphicsContext() = default;
	
	///@copydoc Pomdog::GraphicsContext
	virtual void Clear(Color const& color) = 0;
	
	///@copydoc Pomdog::GraphicsContext
	virtual void Clear(ClearOptions options, Color const& color, float depth, std::int32_t stencil) = 0;
	
	///@copydoc Pomdog::GraphicsContext
	virtual void Present() = 0;
	
	///@~Japanese
	/// @brief インデックスを持たず、インスタンス化されていないプリミティブを描画します。
	/// @param primitiveTopology プリミティブの位相を指定します。
	virtual void Draw(PrimitiveTopology primitiveTopology) = 0;

	///@~Japanese
	/// @brief インデックス付けされた頂点データに基づき、インスタンス化されていないプリミティブを描画します。
	/// @param primitiveTopology プリミティブの位相を指定します。
	/// @param indexBuffer インデックスバッファを指定します。
	/// @param indexCount インデックスの要素数を指定します。
	virtual void DrawIndexed(PrimitiveTopology primitiveTopology,
		std::shared_ptr<IndexBuffer> const& indexBuffer, std::size_t indexCount) = 0;

	///@~Japanese
	/// @brief インデックスを持たない、インスタンス化されたプリミティブを描画します。
	/// @param primitiveTopology プリミティブの位相を指定します。
	/// @param instanceCount レンダリングするプリミティブのインスタンスの数を指定します。
	virtual void DrawInstanced(PrimitiveTopology primitiveTopology, std::size_t instanceCount) = 0;

	///@~Japanese
	/// @brief インデックス付けされた頂点データに基づき、インスタンス化されたプリミティブを描画します。
	/// @param primitiveTopology プリミティブの位相を指定します。
	/// @param indexBuffer インデックスバッファを指定します。
	/// @param indexCount インデックスの要素数を指定します。
	/// @param instanceCount レンダリングするプリミティブのインスタンスの数を指定します。
	virtual void DrawIndexedInstanced(PrimitiveTopology primitiveTopology,
		std::shared_ptr<IndexBuffer> const& indexBuffer, std::size_t indexCount, std::size_t instanceCount) = 0;
		
	///@brief Retrieves the capabilities of a GraphicsContext
	virtual GraphicsCapabilities GetCapabilities() const = 0;

	///@copydoc Pomdog::GraphicsContext
	virtual void SetViewport(Viewport const& viewport) = 0;
	
	///@copydoc Pomdog::GraphicsContext
	virtual Rectangle GetScissorRectangle() const = 0;
	
	///@copydoc Pomdog::GraphicsContext
	virtual void SetScissorRectangle(Rectangle const& rectangle) = 0;
	
	///@copydoc Pomdog::GraphicsContext
	virtual void SetInputLayout(std::shared_ptr<InputLayout> const& inputLayout) = 0;
	
	///@copydoc Pomdog::GraphicsContext
	virtual void SetVertexBuffers(std::vector<std::shared_ptr<VertexBuffer>> const& vertexBuffers) = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVEGRAPHICSCONTEXT_D292A72E_1944_423B_B25A_6A311774AC04_HPP)
