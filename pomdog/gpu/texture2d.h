// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/graphics/forward_declarations.h"
#include "pomdog/graphics/surface_format.h"
#include "pomdog/graphics/texture.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class POMDOG_EXPORT Texture2D : public Texture {
public:
    Texture2D() noexcept;
    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;

    virtual ~Texture2D() override;

    /// Gets the width of the texture data, in pixels.
    [[nodiscard]] virtual std::int32_t GetWidth() const noexcept = 0;

    /// Gets the height of the texture data, in pixels.
    [[nodiscard]] virtual std::int32_t GetHeight() const noexcept = 0;

    /// Gets the mipmap level.
    [[nodiscard]] virtual std::int32_t GetLevelCount() const noexcept = 0;

    /// Gets the format of the pixel data in the texture.
    [[nodiscard]] virtual SurfaceFormat GetFormat() const noexcept = 0;

    /// Sets texture data.
    virtual void SetData(const void* pixelData) = 0;
};

} // namespace pomdog
