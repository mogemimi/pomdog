// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_NATIVEGRAPHICSDEVICE_B06DA321_5506_4E99_8BB0_8704B3B97794_HPP
#define POMDOG_NATIVEGRAPHICSDEVICE_B06DA321_5506_4E99_8BB0_8704B3B97794_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "../Utility/Noncopyable.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>
#include <vector>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeBlendState;
class NativeConstantBuffer;
class NativeDepthStencilState;
class NativeEffectPass;
class NativeEffectReflection;
class NativeIndexBuffer;
class NativeRasterizerState;
class NativeRenderTarget2D;
class NativeSamplerState;
class NativeTexture2D;
class NativeVertexBuffer;
class ShaderBytecode;
class ShaderCompileOptions;

class NativeGraphicsDevice: Noncopyable {
public:
	virtual ~NativeGraphicsDevice() = default;

	virtual ShaderLanguage GetSupportedLanguage() const = 0;

	virtual std::unique_ptr<Shader>
	CreateShader(ShaderBytecode const& shaderBytecode,
		ShaderCompileOptions const& compileOptions) = 0;

	virtual std::unique_ptr<NativeConstantBuffer>
	CreateConstantBuffer(std::uint32_t sizeInBytes) = 0;

	virtual std::unique_ptr<NativeIndexBuffer>
	CreateIndexBuffer(std::uint32_t sizeInBytes, BufferUsage bufferUsage) = 0;

	virtual std::unique_ptr<NativeIndexBuffer>
	CreateIndexBuffer(void const* indices, std::uint32_t sizeInBytes,
		BufferUsage bufferUsage) = 0;

	virtual std::unique_ptr<NativeVertexBuffer>
	CreateVertexBuffer(std::size_t sizeInBytes, BufferUsage bufferUsage) = 0;

	virtual std::unique_ptr<NativeVertexBuffer>
	CreateVertexBuffer(void const* vertices, std::size_t sizeInBytes,
		BufferUsage bufferUsage) = 0;

	virtual std::unique_ptr<NativeBlendState>
	CreateBlendState(BlendDescription const& description) = 0;

	virtual std::unique_ptr<NativeDepthStencilState>
	CreateDepthStencilState(DepthStencilDescription const& description) = 0;

	virtual std::unique_ptr<NativeSamplerState>
	CreateSamplerState(SamplerDescription const& description) = 0;

	virtual std::unique_ptr<NativeRasterizerState>
	CreateRasterizerState(RasterizerDescription const& description) = 0;

	virtual std::unique_ptr<NativeEffectPass>
	CreateEffectPass(EffectPassDescription const& description) = 0;

	virtual std::unique_ptr<NativeEffectReflection>
	CreateEffectReflection(NativeEffectPass & nativeEffectPass) = 0;

	virtual std::unique_ptr<NativeTexture2D>
	CreateTexture2D(std::int32_t width, std::int32_t height,
		std::uint32_t mipmapLevels, SurfaceFormat format) = 0;

	virtual std::unique_ptr<NativeRenderTarget2D>
	CreateRenderTarget2D(std::int32_t width, std::int32_t height,
		std::uint32_t mipmapLevels, SurfaceFormat format, DepthFormat depthStencilFormat) = 0;
};

}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_NATIVEGRAPHICSDEVICE_B06DA321_5506_4E99_8BB0_8704B3B97794_HPP)
