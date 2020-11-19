// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "Texture2DGL4.hpp"
#include "../Utility/Tagged.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <optional>

namespace Pomdog::Detail::GL4 {

using RenderBuffer2DGL4 = Tagged<GLuint, RenderTarget2D>;

class RenderTarget2DGL4 final : public RenderTarget2D {
public:
    ~RenderTarget2DGL4() override;

    [[nodiscard]] std::shared_ptr<Error>
    Initialize(
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format,
        DepthFormat depthStencilFormat,
        std::int32_t multiSampleCount) noexcept;

    /// Gets the width of the texture data, in pixels.
    std::int32_t GetWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    std::int32_t GetHeight() const noexcept override;

    /// Gets the mipmap level.
    std::int32_t GetLevelCount() const noexcept override;

    /// Gets the format of the pixel data in the render target.
    SurfaceFormat GetFormat() const noexcept override;

    /// Gets the format of the pixel data in the depth-stencil buffer.
    DepthFormat GetDepthStencilFormat() const noexcept override;

    /// Gets the size of the texture resource.
    Rectangle GetBounds() const noexcept override;

    /// Copies the pixel data from texture to memory.
    void GetData(void* result, std::size_t offsetInBytes, std::size_t sizeInBytes) const override;

    void BindToFramebuffer(GLenum attachmentPoint);
    void UnbindFromFramebuffer(GLenum attachmentPoint);

    void BindDepthStencilBuffer();

    /// Gets the handle of the native texture resource.
    Texture2DObjectGL4 GetTextureHandle() const noexcept;

private:
    Texture2DGL4 texture;
    std::optional<RenderBuffer2DGL4> renderBuffer;
    std::int32_t pixelWidth = 0;
    std::int32_t pixelHeight = 0;
    std::int32_t levelCount = 0;
    SurfaceFormat format = SurfaceFormat::A8_UNorm;
    DepthFormat depthStencilFormat = DepthFormat::None;
    bool generateMipmap = false;
    bool multiSampleEnabled = false;
};

} // namespace Pomdog::Detail::GL4
