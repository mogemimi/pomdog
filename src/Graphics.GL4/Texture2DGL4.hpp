// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "../Utility/Tagged.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <optional>

namespace Pomdog::Detail::GL4 {

using Texture2DObjectGL4 = Tagged<GLuint, Texture2D>;

class Texture2DGL4 final : public Texture2D {
public:
    ~Texture2DGL4() override;

    [[nodiscard]] std::shared_ptr<Error>
    Initialize(
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format) noexcept;

    /// Gets the width of the texture data, in pixels.
    std::int32_t GetWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    std::int32_t GetHeight() const noexcept override;

    /// Gets the mipmap level.
    std::int32_t GetLevelCount() const noexcept override;

    /// Gets the format of the pixel data in the texture.
    SurfaceFormat GetFormat() const noexcept override;

    /// Sets texture data.
    void SetData(const void* pixelData) override;

    /// Copies the pixel data from texture to memory.
    void GetData(void* result, std::size_t offsetInBytes, std::size_t sizeInBytes) const;

    /// Generates a set of mipmaps.
    void GenerateMipmap();

    /// Gets the handle of the native texture resource.
    Texture2DObjectGL4 GetTextureHandle() const noexcept;

private:
    std::optional<Texture2DObjectGL4> textureObject;
    std::int32_t pixelWidth = 0;
    std::int32_t pixelHeight = 0;
    std::int32_t levelCount = 0;
    SurfaceFormat format = SurfaceFormat::A8_UNorm;
};

} // namespace Pomdog::Detail::GL4
