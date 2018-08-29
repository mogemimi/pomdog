// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "Texture2DGL4.hpp"
#include "../RenderSystem/NativeRenderTarget2D.hpp"
#include "../Utility/Tagged.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <optional>

namespace Pomdog {
namespace Detail {
namespace GL4 {

using RenderBuffer2DGL4 = Tagged<GLuint, RenderTarget2D>;

class RenderTarget2DGL4 final : public NativeRenderTarget2D {
public:
    RenderTarget2DGL4(
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format,
        DepthFormat depthStencilFormat,
        std::int32_t multiSampleCount);

    ~RenderTarget2DGL4();

    void BindToFramebuffer(GLenum attachmentPoint);
    void UnbindFromFramebuffer(GLenum attachmentPoint);

    void BindDepthStencilBuffer();

    const Texture2DObjectGL4& GetTextureHandle() const;

private:
    Texture2DGL4 texture;
    std::optional<RenderBuffer2DGL4> renderBuffer;
    bool generateMipmap;
    bool multiSampleEnabled;
};

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
