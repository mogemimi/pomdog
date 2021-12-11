// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/texture.hpp"
#include "pomdog/math/forward_declarations.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class POMDOG_EXPORT RenderTarget2D : public Texture {
public:
    RenderTarget2D() noexcept;
    RenderTarget2D(const RenderTarget2D&) = delete;
    RenderTarget2D& operator=(const RenderTarget2D&) = delete;

    virtual ~RenderTarget2D() override;

    /// Gets the width of the texture data, in pixels.
    virtual std::int32_t GetWidth() const noexcept = 0;

    /// Gets the height of the texture data, in pixels.
    virtual std::int32_t GetHeight() const noexcept = 0;

    /// Gets the mipmap level.
    virtual std::int32_t GetLevelCount() const noexcept = 0;

    /// Gets the format of the pixel data in the render target.
    virtual SurfaceFormat GetFormat() const noexcept = 0;

    /// Gets the size of the texture resource.
    virtual Rectangle GetBounds() const noexcept = 0;

    /// Copies the pixel data from texture to memory.
    template <typename T>
    void GetData(T* result, std::size_t startIndex, std::size_t elementCount) const
    {
        static_assert(std::is_trivially_copyable_v<T>, "You can only use plain-old-data types.");
        static_assert(std::is_standard_layout_v<T>, "You can only use plain-old-data types.");
        static_assert(!std::is_void_v<T>);
        this->GetData(static_cast<void*>(result), sizeof(T) * startIndex, sizeof(T) * elementCount);
    }

    /// Copies the pixel data from texture to memory.
    virtual void GetData(void* result, std::size_t offsetInBytes, std::size_t sizeInBytes) const = 0;
};

} // namespace pomdog