//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_GRAPHICSCONTEXTGL4_1FF90EA8_4C6A_47DE_A12F_27CA348E4932_HPP
#define POMDOG_GRAPHICSCONTEXTGL4_1FF90EA8_4C6A_47DE_A12F_27CA348E4932_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "OpenGLPrerequisites.hpp"
#include "../RenderSystem/NativeGraphicsContext.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <vector>
#include <memory>

namespace Pomdog {

class GameWindow;

namespace Details {
namespace RenderSystem {
namespace GL4 {

namespace Tags {

struct FrameBufferTag;

}// namespace Tags

using FrameBufferGL4 = Tagged<GLuint, Tags::FrameBufferTag>;

class OpenGLContext;
class ConstantLayoutGL4;
class EffectPassGL4;
class RenderTarget2DGL4;

class GraphicsContextGL4 final: public NativeGraphicsContext {
public:
	GraphicsContextGL4() = delete;

	GraphicsContextGL4(std::shared_ptr<OpenGLContext> const& openGLContext, std::weak_ptr<GameWindow> window);

	~GraphicsContextGL4();

	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void Clear(Color const& color) override;

	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void Clear(ClearOptions options, Color const& color, float depth, std::int32_t stencil) override;

	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void Present() override;

	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void Draw(PrimitiveTopology primitiveTopology, std::uint32_t vertexCount) override;

	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void DrawIndexed(PrimitiveTopology primitiveTopology,
		std::shared_ptr<IndexBuffer> const& indexBuffer, std::uint32_t indexCount) override;

	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void DrawInstanced(PrimitiveTopology primitiveTopology,
		std::uint32_t vertexCount, std::uint32_t instanceCount) override;

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
	void SetVertexBuffers(std::vector<std::shared_ptr<VertexBuffer>> const& vertexBuffers) override;

	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void SetTexture(std::uint32_t index) override;

	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void SetTexture(std::uint32_t index, Texture2D & texture) override;

	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void SetTexture(std::uint32_t index, RenderTarget2D & texture) override;

	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void SetRenderTarget() override;

	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets) override;

	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void SetEffectPass(std::shared_ptr<NativeEffectPass> const& nativeEffectPass) override;

	///@copydoc Pomdog::Details::RenderSystem::NativeGraphicsContext
	void SetConstantBuffers(std::shared_ptr<NativeConstantLayout> const& nativeConstantLayout) override;

private:
	std::vector<std::shared_ptr<VertexBuffer>> vertexBuffers;
	std::shared_ptr<OpenGLContext> nativeContext;
	std::shared_ptr<EffectPassGL4> effectPass;
	std::shared_ptr<ConstantLayoutGL4> constantLayout;
	std::weak_ptr<GameWindow> gameWindow;
	std::vector<Optional<GLenum>> textures;
	Optional<FrameBufferGL4> frameBuffer;
	std::vector<std::shared_ptr<RenderTarget2DGL4>> renderTargets;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_GRAPHICSCONTEXTGL4_1FF90EA8_4C6A_47DE_A12F_27CA348E4932_HPP)
