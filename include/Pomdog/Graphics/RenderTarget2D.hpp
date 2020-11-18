// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Graphics/Texture.hpp"
#include "Pomdog/Math/ForwardDeclarations.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT RenderTarget2D : public Texture {
public:
    RenderTarget2D() noexcept;
    RenderTarget2D(const RenderTarget2D&) = delete;
    RenderTarget2D& operator=(const RenderTarget2D&) = delete;

    virtual ~RenderTarget2D();

    /// Gets the width of the texture data, in pixels.
    virtual std::int32_t GetWidth() const noexcept = 0;

    /// Gets the height of the texture data, in pixels.
    virtual std::int32_t GetHeight() const noexcept = 0;

    /// Gets the mipmap level.
    virtual std::int32_t GetLevelCount() const noexcept = 0;

    /// Gets the format of the pixel data in the render target.
    virtual SurfaceFormat GetFormat() const noexcept = 0;

    /// Gets the format of the pixel data in the depth-stencil buffer.
    virtual DepthFormat GetDepthStencilFormat() const noexcept = 0;

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

} // namespace Pomdog
