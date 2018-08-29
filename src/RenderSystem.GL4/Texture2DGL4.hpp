// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "../RenderSystem/NativeTexture2D.hpp"
#include "../Utility/Tagged.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <optional>

namespace Pomdog {
namespace Detail {
namespace GL4 {

using Texture2DObjectGL4 = Tagged<GLuint, Texture2D>;

class Texture2DGL4 final : public NativeTexture2D {
public:
    Texture2DGL4(
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format);

    ~Texture2DGL4() override;

    void SetData(
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format,
        const void* pixelData) override;

    void GenerateMipmap();

    const Texture2DObjectGL4& GetTextureHandle() const;

private:
    std::optional<Texture2DObjectGL4> textureObject;
};

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
