// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "GraphicsContextGL4.hpp"
#include "OpenGLContext.hpp"
#include "ConstantLayoutGL4.hpp"
#include "EffectPassGL4.hpp"
#include "ErrorChecker.hpp"
#include "IndexBufferGL4.hpp"
#include "InputLayoutGL4.hpp"
#include "RenderTarget2DGL4.hpp"
#include "TypesafeHelperGL4.hpp"
#include "../RenderSystem/GraphicsCapabilities.hpp"
#include "../RenderSystem/NativeRenderTarget2D.hpp"
#include "../RenderSystem/NativeTexture2D.hpp"
#include "../Utility/NumericHelper.hpp"
#include "../Utility/ScopeGuard.hpp"
#include "Pomdog/Graphics/ClearOptions.hpp"
#include "Pomdog/Graphics/IndexBuffer.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Application/GameWindow.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

// logging
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Logging/LogStream.hpp"

#include <cmath>
#include <array>
#include <utility>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {
//-----------------------------------------------------------------------
namespace {

static GLenum ToPrimitiveTopology(PrimitiveTopology primitiveTopology)
{
	switch (primitiveTopology) {
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

	switch (indexElementSize) {
	case IndexElementSize::SixteenBits: return GL_UNSIGNED_SHORT;
	case IndexElementSize::ThirtyTwoBits: return GL_UNSIGNED_INT;
	}
#ifdef _MSC_VER
	return GL_UNSIGNED_INT;
#endif
}
//-----------------------------------------------------------------------
template <typename T>
static GLenum ToTextureUnitIndexGL4(T index)
{
	static_assert(std::is_unsigned<T>::value, "T is unsigned type.");
	static_assert(GL_TEXTURE0  == (GL_TEXTURE0 + 0), "");
	static_assert(GL_TEXTURE1  == (GL_TEXTURE0 + 1), "");
	static_assert(GL_TEXTURE2  == (GL_TEXTURE0 + 2), "");
	static_assert(GL_TEXTURE3  == (GL_TEXTURE0 + 3), "");
	static_assert(GL_TEXTURE4  == (GL_TEXTURE0 + 4), "");
	static_assert(GL_TEXTURE5  == (GL_TEXTURE0 + 5), "");
	static_assert(GL_TEXTURE6  == (GL_TEXTURE0 + 6), "");
	static_assert(GL_TEXTURE7  == (GL_TEXTURE0 + 7), "");
	static_assert(GL_TEXTURE8  == (GL_TEXTURE0 + 8), "");
	static_assert(GL_TEXTURE9  == (GL_TEXTURE0 + 9), "");

	return static_cast<GLenum>(GL_TEXTURE0 + index);
}
//-----------------------------------------------------------------------
template <typename T>
static GLenum ToColorAttachment(T index)
{
	static_assert(std::is_unsigned<T>::value, "T is unsigned type.");
	static_assert(GL_COLOR_ATTACHMENT0  == (GL_COLOR_ATTACHMENT0 + 0), "");
	static_assert(GL_COLOR_ATTACHMENT1  == (GL_COLOR_ATTACHMENT0 + 1), "");
	static_assert(GL_COLOR_ATTACHMENT2  == (GL_COLOR_ATTACHMENT0 + 2), "");
	static_assert(GL_COLOR_ATTACHMENT3  == (GL_COLOR_ATTACHMENT0 + 3), "");
	static_assert(GL_COLOR_ATTACHMENT4  == (GL_COLOR_ATTACHMENT0 + 4), "");
	static_assert(GL_COLOR_ATTACHMENT5  == (GL_COLOR_ATTACHMENT0 + 5), "");
	static_assert(GL_COLOR_ATTACHMENT6  == (GL_COLOR_ATTACHMENT0 + 6), "");
	static_assert(GL_COLOR_ATTACHMENT7  == (GL_COLOR_ATTACHMENT0 + 7), "");
	static_assert(GL_COLOR_ATTACHMENT8  == (GL_COLOR_ATTACHMENT0 + 8), "");
	static_assert(GL_COLOR_ATTACHMENT9  == (GL_COLOR_ATTACHMENT0 + 9), "");

	return static_cast<GLenum>(index + GL_COLOR_ATTACHMENT0);
}
//-----------------------------------------------------------------------
static Optional<FrameBufferGL4> CreateFrameBuffer()
{
	auto const prevFrameBuffer = TypesafeHelperGL4::Get<FrameBufferGL4>();
	ScopeGuard scope([&prevFrameBuffer] {
		glBindFramebuffer(GL_FRAMEBUFFER, prevFrameBuffer.value);
	});

	FrameBufferGL4 frameBuffer;
	glGenFramebuffers(1, frameBuffer.Data());

	#ifdef DEBUG
	ErrorChecker::CheckError("glGenFramebuffers", __FILE__, __LINE__);
	#endif

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.value);

	// Check framebuffer
	auto const status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (GL_FRAMEBUFFER_UNSUPPORTED == status)
	{
		return OptionalType::NullOptional;
	}

	return std::move(frameBuffer);
}

}// unnamed namespace
//-----------------------------------------------------------------------
template<>
struct TypesafeHelperGL4::OpenGLGetTraits<FrameBufferGL4> {
	constexpr static GLenum bufferObjectBinding = GL_FRAMEBUFFER_BINDING;
};
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - GraphicsContextGL4
#endif
//-----------------------------------------------------------------------
GraphicsContextGL4::GraphicsContextGL4(std::shared_ptr<OpenGLContext> const& openGLContextIn, std::weak_ptr<GameWindow> windowIn)
	: nativeContext(openGLContextIn)
	, gameWindow(std::move(windowIn))
{
	auto version = reinterpret_cast<char const*>(glGetString(GL_VERSION));
	Log::Stream(LogLevel::Internal) << "OpenGL Version: " << version;

	auto capabilities = GetCapabilities();
	if (capabilities.SamplerSlotCount > 0)
	{
		textures.resize(capabilities.SamplerSlotCount);
	}

	glFrontFace(GL_CW);

	#ifdef DEBUG
	ErrorChecker::CheckError("glFrontFace", __FILE__, __LINE__);
	#endif

	frameBuffer = CreateFrameBuffer();
}
//-----------------------------------------------------------------------
GraphicsContextGL4::~GraphicsContextGL4()
{
	renderTargets.clear();

	if (frameBuffer)
	{
		glDeleteFramebuffers(1, frameBuffer->Data());
		frameBuffer = OptionalType::NullOptional;
	}
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::Clear(Color const& color)
{
	auto colorVector = color.ToVector4();
	glClearColor(colorVector.X, colorVector.Y, colorVector.Z, colorVector.W);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	#ifdef DEBUG
	RenderSystem::GL4::ErrorChecker::CheckError("glClear", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::Clear(ClearOptions options, Color const& color, float depth, std::int32_t stencil)
{
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
		auto colorVector = color.ToVector4();
		glClearColor(colorVector.X, colorVector.Y, colorVector.Z, colorVector.W);
	}

	glClear(mask);

	#ifdef DEBUG
	ErrorChecker::CheckError("glClear", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::Present()
{
	nativeContext->SwapBuffers();

	#ifdef DEBUG
	RenderSystem::GL4::ErrorChecker::CheckError("SwapBuffers", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::Draw(PrimitiveTopology primitiveTopology, std::size_t vertexCount)
{
	// Bind input-layout to the input-assembler stage:
	POMDOG_ASSERT(effectPass);
	auto inputLayout = effectPass->GetInputLayout();

	POMDOG_ASSERT(inputLayout);
	POMDOG_ASSERT(!vertexBuffers.empty());
	inputLayout->Apply(vertexBuffers);

	// Use shader program:
	POMDOG_ASSERT(effectPass);
	effectPass->ApplyShaders();

	// Apply constant layout:
	POMDOG_ASSERT(constantLayout);
	constantLayout->Apply();

	// Draw:
	POMDOG_ASSERT(!vertexBuffers.empty());
	POMDOG_ASSERT(vertexBuffers.front());
	POMDOG_ASSERT(vertexCount <= vertexBuffers.front()->VertexCount());

	glDrawArrays(
		ToPrimitiveTopology(primitiveTopology),
		0,
		NumericHelper::Cast<GLsizei>(vertexCount));

	#ifdef DEBUG
	ErrorChecker::CheckError("glDrawArrays", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::DrawIndexed(PrimitiveTopology primitiveTopology,
	std::shared_ptr<IndexBuffer> const& indexBuffer, std::size_t indexCount)
{
	// Bind input-layout to the input-assembler stage:
	POMDOG_ASSERT(effectPass);
	auto inputLayout = effectPass->GetInputLayout();

	POMDOG_ASSERT(inputLayout);
	POMDOG_ASSERT(!vertexBuffers.empty());
	inputLayout->Apply(vertexBuffers);

	// Use shader program:
	POMDOG_ASSERT(effectPass);
	effectPass->ApplyShaders();

	// Apply constant layout:
	POMDOG_ASSERT(constantLayout);
	constantLayout->Apply();

	// Bind index-buffer:
	POMDOG_ASSERT(indexCount > 0);
	POMDOG_ASSERT(indexCount <= indexBuffer->IndexCount());
	POMDOG_ASSERT(indexBuffer);

	auto nativeIndexBuffer = dynamic_cast<IndexBufferGL4*>(indexBuffer->NativeIndexBuffer());
	POMDOG_ASSERT(nativeIndexBuffer != nullptr);

	nativeIndexBuffer->BindBuffer();

	glDrawElements(
		ToPrimitiveTopology(primitiveTopology),
		NumericHelper::Cast<GLsizei>(indexCount),
		ToIndexElementType(indexBuffer->ElementSize()),
		nullptr);

	#ifdef DEBUG
	ErrorChecker::CheckError("glDrawElements", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::DrawInstanced(PrimitiveTopology primitiveTopology,
	std::size_t vertexCount, std::size_t instanceCount)
{
	// Bind input-layout to the input-assembler stage:
	POMDOG_ASSERT(effectPass);
	auto inputLayout = effectPass->GetInputLayout();

	POMDOG_ASSERT(inputLayout);
	POMDOG_ASSERT(!vertexBuffers.empty());
	inputLayout->Apply(vertexBuffers);

	// Use shader program:
	POMDOG_ASSERT(effectPass);
	effectPass->ApplyShaders();

	// Apply constant layout:
	POMDOG_ASSERT(constantLayout);
	constantLayout->Apply();

	// Draw
	POMDOG_ASSERT(!vertexBuffers.empty());
	POMDOG_ASSERT(vertexBuffers.front());
	POMDOG_ASSERT(vertexCount <= vertexBuffers.front()->VertexCount());
	POMDOG_ASSERT(0 < instanceCount);

	glDrawArraysInstanced(
		ToPrimitiveTopology(primitiveTopology),
		0,
		NumericHelper::Cast<GLsizei>(vertexCount),
		NumericHelper::Cast<GLsizei>(instanceCount));

	#ifdef DEBUG
	ErrorChecker::CheckError("glDrawArraysInstanced", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::DrawIndexedInstanced(PrimitiveTopology primitiveTopology,
	std::shared_ptr<IndexBuffer> const& indexBuffer, std::size_t indexCount, std::size_t instanceCount)
{
	// Bind input-layout to the input-assembler stage:
	POMDOG_ASSERT(effectPass);
	auto inputLayout = effectPass->GetInputLayout();

	POMDOG_ASSERT(inputLayout);
	POMDOG_ASSERT(!vertexBuffers.empty());
	inputLayout->Apply(vertexBuffers);

	// Use shader program:
	POMDOG_ASSERT(effectPass);
	effectPass->ApplyShaders();

	// Apply constant layout:
	POMDOG_ASSERT(constantLayout);
	constantLayout->Apply();

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
		NumericHelper::Cast<GLsizei>(indexCount),
		ToIndexElementType(indexBuffer->ElementSize()),
		nullptr,
		NumericHelper::Cast<GLsizei>(instanceCount));

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

	if (renderTargets.empty()) {
		if (auto window = gameWindow.lock()) {
			viewportY = window->ClientBounds().Height - (viewport.TopLeftY() + viewport.Height());
		}
	}

	glViewport(viewport.TopLeftX(), viewportY, viewport.Width(), viewport.Height());

	#ifdef DEBUG
	ErrorChecker::CheckError("glViewport", __FILE__, __LINE__);
	#endif

	static_assert(std::is_same<GLfloat, decltype(viewport.MinDepth)>::value
		&& std::is_same<GLfloat, decltype(viewport.MaxDepth)>::value,
		"NOTE: You can use glDepthRange instead of glDepthRangef");

	POMDOG_ASSERT(!std::isinf(viewport.MinDepth));
	POMDOG_ASSERT(!std::isinf(viewport.MaxDepth));
	POMDOG_ASSERT(!std::isnan(viewport.MinDepth));
	POMDOG_ASSERT(!std::isnan(viewport.MaxDepth));

	POMDOG_ASSERT_MESSAGE(glDepthRangef != nullptr, "glDepthRangef() not found");
	glDepthRangef(viewport.MinDepth, viewport.MaxDepth);

	#ifdef DEBUG
	ErrorChecker::CheckError("glDepthRangef", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
Rectangle GraphicsContextGL4::GetScissorRectangle() const
{
	std::array<GLint, 4> scissorBox;
	glGetIntegerv(GL_SCISSOR_BOX, scissorBox.data());

	Rectangle rect {scissorBox[0], scissorBox[1], scissorBox[2], scissorBox[3]};

	if (renderTargets.empty()) {
		if (auto window = gameWindow.lock()) {
			rect.Y = window->ClientBounds().Height - (rect.Y + rect.Height);
		}
	}

	return std::move(rect);
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::SetScissorRectangle(Rectangle const& rectangle)
{
	POMDOG_ASSERT(rectangle.Width > 0);
	POMDOG_ASSERT(rectangle.Height > 0);

	GLint lowerLeftCornerY = rectangle.Y;

	if (renderTargets.empty()) {
		if (auto window = gameWindow.lock()) {
			lowerLeftCornerY = window->ClientBounds().Height - (rectangle.Y + rectangle.Height);
		}
	}

	glScissor(rectangle.X, lowerLeftCornerY, rectangle.Width, rectangle.Height);

	#ifdef DEBUG
	ErrorChecker::CheckError("glScissor", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::SetVertexBuffers(std::vector<std::shared_ptr<VertexBuffer>> const& vertexBuffersIn)
{
	POMDOG_ASSERT(!vertexBuffersIn.empty());
	this->vertexBuffers = vertexBuffersIn;
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::SetEffectPass(std::shared_ptr<NativeEffectPass> const& effectPassIn)
{
	POMDOG_ASSERT(effectPassIn);
	auto nativeEffectPass = std::dynamic_pointer_cast<EffectPassGL4>(effectPassIn);

	POMDOG_ASSERT(nativeEffectPass);
	this->effectPass = nativeEffectPass;
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::SetConstantBuffers(std::shared_ptr<NativeConstantLayout> const& constantLayoutIn)
{
	POMDOG_ASSERT(constantLayoutIn);
	auto nativeConstantLayout = std::dynamic_pointer_cast<ConstantLayoutGL4>(constantLayoutIn);

	POMDOG_ASSERT(nativeConstantLayout);
	this->constantLayout = nativeConstantLayout;
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::SetTexture(int textureUnit)
{
	POMDOG_ASSERT(!textures.empty());
	POMDOG_ASSERT(textureUnit >= 0);
	POMDOG_ASSERT(textureUnit < static_cast<int>(textures.size()));

	if (textures[textureUnit])
	{
		glActiveTexture(ToTextureUnitIndexGL4(textureUnit));

		#ifdef DEBUG
		ErrorChecker::CheckError("glActiveTexture", __FILE__, __LINE__);
		#endif

		glBindTexture(*textures[textureUnit], 0);

		#ifdef DEBUG
		ErrorChecker::CheckError("glBindTexture", __FILE__, __LINE__);
		#endif
	}

	textures[textureUnit] = OptionalType::NullOptional;
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::SetTexture(int textureUnit, Texture2D & textureIn)
{
	POMDOG_ASSERT(!textures.empty());
	POMDOG_ASSERT(textureUnit >= 0);
	POMDOG_ASSERT(textureUnit < static_cast<int>(textures.size()));

	constexpr GLenum textureType = GL_TEXTURE_2D;

	if (textures[textureUnit] && *textures[textureUnit] != textureType)
	{
		// Unbind texture
		SetTexture(textureUnit);
	}

	textures[textureUnit] = textureType;

	POMDOG_ASSERT(textureIn.NativeTexture2D());
	textureIn.NativeTexture2D()->Apply(textureUnit);
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::SetTexture(int textureUnit, RenderTarget2D & textureIn)
{
	POMDOG_ASSERT(!textures.empty());
	POMDOG_ASSERT(textureUnit >= 0);
	POMDOG_ASSERT(textureUnit < static_cast<int>(textures.size()));

	constexpr GLenum textureType = GL_TEXTURE_2D;

	if (textures[textureUnit] && *textures[textureUnit] != textureType)
	{
		// Unbind texture
		SetTexture(textureUnit);
	}

	textures[textureUnit] = textureType;

	POMDOG_ASSERT(textureIn.NativeRenderTarget2D());
	textureIn.NativeRenderTarget2D()->Apply(textureUnit);
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::SetRenderTarget()
{
	POMDOG_ASSERT(frameBuffer);

	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->value);

	#ifdef DEBUG
	ErrorChecker::CheckError("glBindFramebuffer", __FILE__, __LINE__);
	#endif

	// Unbind render targets
	{
		std::size_t index = 0;
		for (auto const& renderTarget: renderTargets)
		{
			POMDOG_ASSERT(renderTarget);
			renderTarget->UnbindFromFramebuffer(ToColorAttachment(index));
			++index;
		}
	}
	renderTargets.clear();

	// Unbind depth stencil buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);

	#ifdef DEBUG
	ErrorChecker::CheckError("glFramebufferRenderbuffer", __FILE__, __LINE__);
	#endif

	// Bind default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	#ifdef DEBUG
	ErrorChecker::CheckError("glBindFramebuffer", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargetsIn)
{
	POMDOG_ASSERT(!renderTargetsIn.empty());
	POMDOG_ASSERT(frameBuffer);

	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->value);

	#ifdef DEBUG
	ErrorChecker::CheckError("glBindFramebuffer", __FILE__, __LINE__);
	#endif

	// Unbind render targets
	{
		std::size_t index = 0;
		for (auto const& renderTarget: renderTargets)
		{
			POMDOG_ASSERT(renderTarget);
			renderTarget->UnbindFromFramebuffer(ToColorAttachment(index));
			++index;
		}
	}
	renderTargets.clear();

	// Unbind depth stencil buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);

	#ifdef DEBUG
	ErrorChecker::CheckError("glFramebufferRenderbuffer", __FILE__, __LINE__);
	#endif

	std::vector<GLenum> attachments;
	attachments.reserve(renderTargetsIn.size());
	renderTargets.reserve(renderTargetsIn.size());

	// Attach textures
	std::uint32_t index = 0;
	for (auto const& renderTarget: renderTargetsIn)
	{
		POMDOG_ASSERT(renderTarget);
		POMDOG_ASSERT(renderTarget->NativeRenderTarget2D());
		auto const nativeRenderTarget = dynamic_cast<RenderTarget2DGL4*>(renderTarget->NativeRenderTarget2D());

		POMDOG_ASSERT(nativeRenderTarget);
		nativeRenderTarget->BindToFramebuffer(ToColorAttachment(index));

		renderTargets.emplace_back(renderTarget, nativeRenderTarget);

		attachments.push_back(ToColorAttachment(index));
		++index;
	}

	// Attach depth stencil buffer
	{
		POMDOG_ASSERT(renderTargets.front());
		auto const& renderTarget = renderTargets.front();

		POMDOG_ASSERT(renderTarget);
		renderTarget->BindDepthStencilBuffer();
	}

	// Check framebuffer status
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		// FUS RO DAH!
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to make complete framebuffer.");
	}

	POMDOG_ASSERT(!attachments.empty());
	POMDOG_ASSERT(std::numeric_limits<GLsizei>::max() >= attachments.size());
	glDrawBuffers(static_cast<GLsizei>(attachments.size()), attachments.data());

	#ifdef DEBUG
	ErrorChecker::CheckError("glDrawBuffers", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog
