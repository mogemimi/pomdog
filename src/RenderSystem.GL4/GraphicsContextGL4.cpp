//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "GraphicsContextGL4.hpp"
#include <array>
#include <utility>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Utility/Exception.hpp>
#include <Pomdog/Math/Color.hpp>
#include <Pomdog/Math/Rectangle.hpp>
#include <Pomdog/Graphics/ClearOptions.hpp>
#include <Pomdog/Graphics/IndexBuffer.hpp>
#include <Pomdog/Graphics/InputLayout.hpp>
#include <Pomdog/Graphics/PrimitiveTopology.hpp>
#include <Pomdog/Graphics/VertexBuffer.hpp>
#include <Pomdog/Graphics/Viewport.hpp>
#include <Pomdog/Application/GameWindow.hpp>
#include "../RenderSystem/GraphicsCapabilities.hpp"
#include "OpenGLContext.hpp"
#include "EffectPassGL4.hpp"
#include "ErrorChecker.hpp"
#include "IndexBufferGL4.hpp"
#include "InputLayoutGL4.hpp"

// logging
#include <Pomdog/Logging/Log.hpp>
#include <Pomdog/Logging/LogStream.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {
//-----------------------------------------------------------------------
namespace {

static GLenum ToPrimitiveTopology(PrimitiveTopology primitiveTopology)
{
	switch (primitiveTopology)
	{
	case PrimitiveTopology::TriangleList: return GL_TRIANGLES;
	case PrimitiveTopology::TriangleStrip: return GL_TRIANGLE_STRIP;
	case PrimitiveTopology::LineList: return GL_LINES;
	case PrimitiveTopology::LineStrip: return GL_LINE_STRIP;
	}
#ifdef _MSC_VER
	return GL_TRIANGLES;
#endif
}
//-----------------------------------------------------------------------
static GLenum ToIndexElementType(IndexElementSize indexElementSize)
{
	static_assert(sizeof(GLushort) == 2, "GLushort is not SixteenBits.");
	static_assert(sizeof(GLuint) == 4, "GLuint is not ThirtyTwoBits.");

	switch (indexElementSize)
	{
	case IndexElementSize::SixteenBits: return GL_UNSIGNED_SHORT;
	case IndexElementSize::ThirtyTwoBits: return GL_UNSIGNED_INT;
	}
#ifdef _MSC_VER
	return GL_UNSIGNED_INT;
#endif
}

}// namespace
//-----------------------------------------------------------------------
GraphicsContextGL4::GraphicsContextGL4(std::shared_ptr<OpenGLContext> openGLContext, std::weak_ptr<GameWindow> window)
	: nativeContext(std::move(openGLContext))
	, gameWindow(std::move(window))
{
	auto version = reinterpret_cast<char const*>(glGetString(GL_VERSION));
	Log::Stream(LoggingLevel::Internal) << "OpenGL Version: " << version << "\n";
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::Clear(Color const& color)
{
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::Clear(ClearOptions options, Color const& color, float depth, std::int32_t stencil)
{
	glClearColor(color.r, color.g, color.b, color.a);
	
	GLbitfield mask = 0;
	
	if ((options | ClearOptions::DepthBuffer) == options) {
		mask |= GL_DEPTH_BUFFER_BIT;
		glClearDepth(static_cast<GLclampd>(depth));
	}
	if ((options | ClearOptions::Stencil) == options) {
		mask |= GL_STENCIL_BUFFER_BIT;
		glClearStencil(stencil);
	}
	if ((options | ClearOptions::RenderTarget) == options) {
		mask |= GL_COLOR_BUFFER_BIT;
		glClearColor(color.r, color.g, color.b, color.a);
	}
	
	glClear(mask);
	
	#ifdef DEBUG
	ErrorChecker::CheckError("glClear", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::Present()
{
	glFlush();

	nativeContext->SwapBuffers();
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::Draw(PrimitiveTopology primitiveTopology)
{
	// Bind input-layout to the input-assembler stage:
	POMDOG_ASSERT(inputLayout);
	POMDOG_ASSERT(!vertexBuffers.empty());
	inputLayout->Apply(vertexBuffers);
	
	// Use shader program:
	POMDOG_ASSERT(effectPass);
	effectPass->ApplyShaders();
	
	// Draw:
	POMDOG_ASSERT(!vertexBuffers.empty());
	POMDOG_ASSERT(vertexBuffers.front());
	
	glDrawArrays(
		ToPrimitiveTopology(primitiveTopology),
		0,
		vertexBuffers.front()->VertexCount()
	);
	
	#ifdef DEBUG
	ErrorChecker::CheckError("glDrawArrays", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::DrawIndexed(PrimitiveTopology primitiveTopology,
	std::shared_ptr<IndexBuffer> const& indexBuffer, std::uint32_t indexCount)
{
	// Bind input-layout to the input-assembler stage:
	POMDOG_ASSERT(inputLayout);
	POMDOG_ASSERT(!vertexBuffers.empty());
	inputLayout->Apply(vertexBuffers);
	
	// Use shader program:
	POMDOG_ASSERT(effectPass);
	effectPass->ApplyShaders();

	// Bind index-buffer:
	POMDOG_ASSERT(indexCount > 0);
	POMDOG_ASSERT(indexCount <= indexBuffer->IndexCount());
	POMDOG_ASSERT(indexBuffer);

	auto nativeIndexBuffer = dynamic_cast<IndexBufferGL4*>(indexBuffer->NativeIndexBuffer());
	POMDOG_ASSERT(nativeIndexBuffer != nullptr);

	nativeIndexBuffer->BindBuffer();

	glDrawElements(
		ToPrimitiveTopology(primitiveTopology),
		indexCount,
		ToIndexElementType(indexBuffer->ElementSize()),
		nullptr
	);
	
	#ifdef DEBUG
	ErrorChecker::CheckError("glDrawElements", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::DrawInstanced(PrimitiveTopology primitiveTopology, std::uint32_t instanceCount)
{
	// Bind input-layout to the input-assembler stage:
	POMDOG_ASSERT(inputLayout);
	POMDOG_ASSERT(!vertexBuffers.empty());
	inputLayout->Apply(vertexBuffers);
	
	// Use shader program:
	POMDOG_ASSERT(effectPass);
	effectPass->ApplyShaders();

	// Draw
	POMDOG_ASSERT(!vertexBuffers.empty());
	POMDOG_ASSERT(vertexBuffers.front());
	POMDOG_ASSERT(0 < instanceCount);

	glDrawArraysInstanced(
		ToPrimitiveTopology(primitiveTopology),
		0,
		vertexBuffers.front()->VertexCount(),
		instanceCount
	);
	
	#ifdef DEBUG
	ErrorChecker::CheckError("glDrawArraysInstanced", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::DrawIndexedInstanced(PrimitiveTopology primitiveTopology,
	std::shared_ptr<IndexBuffer> const& indexBuffer, std::uint32_t indexCount, std::uint32_t instanceCount)
{
	// Bind input-layout to the input-assembler stage:
	POMDOG_ASSERT(inputLayout);
	POMDOG_ASSERT(!vertexBuffers.empty());
	inputLayout->Apply(vertexBuffers);
	
	// Use shader program:
	POMDOG_ASSERT(effectPass);
	effectPass->ApplyShaders();

	// Bind index-buffer:
	POMDOG_ASSERT(indexCount > 0);
	POMDOG_ASSERT(indexCount <= indexBuffer->IndexCount());
	POMDOG_ASSERT(indexBuffer);

	auto nativeIndexBuffer = dynamic_cast<IndexBufferGL4*>(indexBuffer->NativeIndexBuffer());
	POMDOG_ASSERT(nativeIndexBuffer != nullptr);

	nativeIndexBuffer->BindBuffer();

	// Draw
	POMDOG_ASSERT(0 < instanceCount);

	glDrawElementsInstanced(
		ToPrimitiveTopology(primitiveTopology),
		indexCount,
		ToIndexElementType(indexBuffer->ElementSize()),
		nullptr,
		instanceCount
	);
	
	#ifdef DEBUG
	ErrorChecker::CheckError("glDrawElementsInstanced", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
GraphicsCapabilities GraphicsContextGL4::GetCapabilities() const
{
	GraphicsCapabilities capabilities;

	GLint maxTextureUnits = 0;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
	POMDOG_ASSERT(maxTextureUnits > 0);
	
	capabilities.SamplerSlotCount = maxTextureUnits;
	return capabilities;
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::SetViewport(Viewport const& viewport)
{
	POMDOG_ASSERT(viewport.Width() > 0);
	POMDOG_ASSERT(viewport.Height() > 0);

	GLint viewportY = viewport.TopLeftY();
	if (auto window = gameWindow.lock()) {
		viewportY = window->ClientBounds().height - (viewport.TopLeftY() + viewport.Height());
	}

	// Notes: glViewport(x, y, width, height)
	// "Specify the lower left corner of the viewport rectangle, in pixels."
	glViewport(
		static_cast<GLint>(viewport.TopLeftX()),
		static_cast<GLint>(viewportY),
		static_cast<GLsizei>(viewport.Width()),
		static_cast<GLsizei>(viewport.Height())
	);
}
//-----------------------------------------------------------------------
Rectangle GraphicsContextGL4::GetScissorRectangle() const
{
	std::array<GLint, 4> scissorBox;
	glGetIntegerv(GL_SCISSOR_BOX, scissorBox.data());
	return Rectangle{ scissorBox[0], scissorBox[1], scissorBox[2], scissorBox[3] };
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::SetScissorRectangle(Rectangle const& rectangle)
{
	glScissor(rectangle.x, rectangle.y, rectangle.width, rectangle.height);
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::SetInputLayout(std::shared_ptr<InputLayout> const& inputLayoutIn)
{
	POMDOG_ASSERT(inputLayoutIn);
	
	auto nativeInputLayout = dynamic_cast<InputLayoutGL4*>(inputLayoutIn->NativeInputLayout());
	POMDOG_ASSERT(nativeInputLayout);

	if (nativeInputLayout != nullptr) {
		this->inputLayout = std::shared_ptr<InputLayoutGL4>(inputLayoutIn, nativeInputLayout);
	}
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::SetVertexBuffers(std::vector<std::shared_ptr<VertexBuffer>> const& vertexBuffersIn)
{
	POMDOG_ASSERT(!vertexBuffersIn.empty());
	this->vertexBuffers = vertexBuffersIn;
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::SetEffectPass(std::shared_ptr<EffectPassGL4> const& nativeEffectPassIn)
{
	POMDOG_ASSERT(nativeEffectPassIn);
	this->effectPass = nativeEffectPassIn;
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
