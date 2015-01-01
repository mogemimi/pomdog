//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVEGRAPHICSCONTEXT_D292A72E_1944_423B_B25A_6A311774AC04_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVEGRAPHICSCONTEXT_D292A72E_1944_423B_B25A_6A311774AC04_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "../Utility/Noncopyable.hpp"
#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>
#include <Pomdog/Math/detail/ForwardDeclarations.hpp>
#include <vector>
#include <memory>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class GraphicsCapabilities;
class NativeEffectPass;
class NativeConstantLayout;

class NativeGraphicsContext: Noncopyable {
public:
	virtual ~NativeGraphicsContext() = default;
	
	///@copydoc Pomdog::GraphicsContext
	virtual void Clear(Color const& color) = 0;
	
	///@copydoc Pomdog::GraphicsContext
	virtual void Clear(ClearOptions options, Color const& color, float depth, std::int32_t stencil) = 0;
	
	///@copydoc Pomdog::GraphicsContext
	virtual void Present() = 0;
	
	///@copydoc Pomdog::GraphicsContext
	virtual void Draw(PrimitiveTopology primitiveTopology, std::uint32_t vertexCount) = 0;

	///@copydoc Pomdog::GraphicsContext
	virtual void DrawIndexed(PrimitiveTopology primitiveTopology,
		std::shared_ptr<IndexBuffer> const& indexBuffer, std::uint32_t indexCount) = 0;

	///@copydoc Pomdog::GraphicsContext
	virtual void DrawInstanced(PrimitiveTopology primitiveTopology, std::uint32_t vertexCount, std::uint32_t instanceCount) = 0;

	///@copydoc Pomdog::GraphicsContext
	virtual void DrawIndexedInstanced(PrimitiveTopology primitiveTopology,
		std::shared_ptr<IndexBuffer> const& indexBuffer, std::uint32_t indexCount, std::uint32_t instanceCount) = 0;
	
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
	
	///@copydoc Pomdog::GraphicsContext
	virtual void SetTexture(std::uint32_t index) = 0;

	///@copydoc Pomdog::GraphicsContext
	virtual void SetTexture(std::uint32_t index, Texture2D & texture) = 0;
	
	///@copydoc Pomdog::GraphicsContext
	virtual void SetTexture(std::uint32_t index, RenderTarget2D & texture) = 0;
	
	///@copydoc Pomdog::GraphicsContext
	virtual void SetRenderTarget() = 0;

	///@copydoc Pomdog::GraphicsContext
	virtual void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets) = 0;
	
	///@details low-level API
	virtual void SetEffectPass(std::shared_ptr<NativeEffectPass> const& nativeEffectPass) = 0;
	
	///@details low-level API
	virtual void SetConstantBuffers(std::shared_ptr<NativeConstantLayout> const& nativeConstantLayout) = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVEGRAPHICSCONTEXT_D292A72E_1944_423B_B25A_6A311774AC04_HPP)
