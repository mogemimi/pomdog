//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_GL4_GRAPHICSCONTEXTGL4_1FF90EA8_4C6A_47DE_A12F_27CA348E4932_HPP
#define POMDOG_SRC_GL4_GRAPHICSCONTEXTGL4_1FF90EA8_4C6A_47DE_A12F_27CA348E4932_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include "OpenGLPrerequisites.hpp"
#include "../RenderSystem/NativeGraphicsContext.hpp"

namespace Pomdog {

class GameWindow;

namespace Details {
namespace RenderSystem {
namespace GL4 {

class OpenGLContext;
class EffectPassGL4;
class InputLayoutGL4;

class GraphicsContextGL4 final: public NativeGraphicsContext
{
public:
	GraphicsContextGL4() = delete;

	GraphicsContextGL4(std::shared_ptr<OpenGLContext> openGLContext, std::weak_ptr<GameWindow> window);
	
	~GraphicsContextGL4() = default;
	
	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void Clear(Color const& color) override;
	
	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void Clear(ClearOptions options, Color const& color, float depth, std::int32_t stencil) override;
	
	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void Present() override;
	
	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void Draw(PrimitiveTopology primitiveTopology) override;

	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void DrawIndexed(PrimitiveTopology primitiveTopology,
		std::shared_ptr<IndexBuffer> const& indexBuffer, std::uint32_t indexCount) override;

	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void DrawInstanced(PrimitiveTopology primitiveTopology, std::uint32_t instanceCount) override;

	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void DrawIndexedInstanced(PrimitiveTopology primitiveTopology,
		std::shared_ptr<IndexBuffer> const& indexBuffer, std::uint32_t indexCount, std::uint32_t instanceCount) override;
	
	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	GraphicsCapabilities GetCapabilities() const override;
	
	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void SetViewport(Viewport const& viewport) override;
	
	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	Rectangle GetScissorRectangle() const override;
	
	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void SetScissorRectangle(Rectangle const& rectangle) override;
	
	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void SetInputLayout(std::shared_ptr<InputLayout> const& inputLayout) override;
	
	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void SetVertexBuffers(std::vector<std::shared_ptr<VertexBuffer>> const& vertexBuffers) override;
	
	///@details EffectPass::Apply から呼ばれます。
	void SetEffectPass(std::shared_ptr<EffectPassGL4> const& nativeEffectPass);
	
private:
	std::vector<std::shared_ptr<VertexBuffer>> vertexBuffers;
	std::shared_ptr<OpenGLContext> nativeContext;
	std::shared_ptr<InputLayoutGL4> inputLayout;
	std::shared_ptr<EffectPassGL4> effectPass;
	std::weak_ptr<GameWindow> gameWindow;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_GL4_GRAPHICSCONTEXTGL4_1FF90EA8_4C6A_47DE_A12F_27CA348E4932_HPP)
