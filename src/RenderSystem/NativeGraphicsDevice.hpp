// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_NATIVEGRAPHICSDEVICE_B06DA321_HPP
#define POMDOG_NATIVEGRAPHICSDEVICE_B06DA321_HPP

#include "../Utility/Noncopyable.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>
#include <vector>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeBuffer;
class NativeEffectPass;
class NativeEffectReflection;
class NativeRenderTarget2D;
class NativeSamplerState;
class NativeTexture2D;
class ShaderBytecode;
class ShaderCompileOptions;

class NativeGraphicsDevice: Noncopyable {
public:
    virtual ~NativeGraphicsDevice() = default;

    virtual ShaderLanguage GetSupportedLanguage() const = 0;

    virtual std::unique_ptr<Shader>
    CreateShader(ShaderBytecode const& shaderBytecode,
        ShaderCompileOptions const& compileOptions) = 0;

    virtual std::unique_ptr<NativeBuffer>
    CreateConstantBuffer(std::size_t sizeInBytes) = 0;

    virtual std::unique_ptr<NativeBuffer>
    CreateIndexBuffer(std::size_t sizeInBytes, BufferUsage bufferUsage) = 0;

    virtual std::unique_ptr<NativeBuffer>
    CreateIndexBuffer(void const* indices, std::size_t sizeInBytes,
        BufferUsage bufferUsage) = 0;

    virtual std::unique_ptr<NativeBuffer>
    CreateVertexBuffer(std::size_t sizeInBytes, BufferUsage bufferUsage) = 0;

    virtual std::unique_ptr<NativeBuffer>
    CreateVertexBuffer(void const* vertices, std::size_t sizeInBytes,
        BufferUsage bufferUsage) = 0;

    virtual std::unique_ptr<NativeSamplerState>
    CreateSamplerState(SamplerDescription const& description) = 0;

    virtual std::unique_ptr<NativeEffectPass>
    CreateEffectPass(EffectPassDescription const& description) = 0;

    virtual std::unique_ptr<NativeEffectReflection>
    CreateEffectReflection(NativeEffectPass & nativeEffectPass) = 0;

    virtual std::unique_ptr<NativeTexture2D>
    CreateTexture2D(std::int32_t width, std::int32_t height,
        std::uint32_t mipmapLevels, SurfaceFormat format) = 0;

    virtual std::unique_ptr<NativeRenderTarget2D>
    CreateRenderTarget2D(std::int32_t width, std::int32_t height,
        std::uint32_t mipmapLevels, SurfaceFormat format,
        DepthFormat depthStencilFormat) = 0;
};

} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_NATIVEGRAPHICSDEVICE_B06DA321_HPP
