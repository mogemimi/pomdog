// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/texture.h"
#include "pomdog/math/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

class POMDOG_EXPORT RenderTarget2D : public Texture {
public:
    RenderTarget2D() noexcept;
    RenderTarget2D(const RenderTarget2D&) = delete;
    RenderTarget2D& operator=(const RenderTarget2D&) = delete;

    virtual ~RenderTarget2D() override;

    /// Gets the width of the texture data, in pixels.
    [[nodiscard]] virtual std::int32_t
    getWidth() const noexcept = 0;

    /// Gets the height of the texture data, in pixels.
    [[nodiscard]] virtual std::int32_t
    getHeight() const noexcept = 0;

    /// Gets the mipmap level.
    [[nodiscard]] virtual std::int32_t
    getLevelCount() const noexcept = 0;

    /// Gets the format of the pixel data in the render target.
    [[nodiscard]] virtual PixelFormat
    getFormat() const noexcept = 0;

    /// Gets the size of the texture resource.
    [[nodiscard]] virtual Rectangle
    getBounds() const noexcept = 0;

    /// Copies the pixel data from texture to memory.
    template <typename T>
    void getData(T* result, std::size_t startIndex, std::size_t elementCount) const
    {
        static_assert(std::is_trivially_copyable_v<T>, "You can only use plain-old-data types.");
        static_assert(std::is_standard_layout_v<T>, "You can only use plain-old-data types.");
        static_assert(!std::is_void_v<T>);
        getData(static_cast<void*>(result), sizeof(T) * startIndex, sizeof(T) * elementCount);
    }

    /// Copies the pixel data from texture to memory.
    virtual void getData(void* result, std::size_t offsetInBytes, std::size_t sizeInBytes) const = 0;
};

} // namespace pomdog::gpu
