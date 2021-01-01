// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/Texture.hpp"
#include "Pomdog/Math/ForwardDeclarations.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

class POMDOG_EXPORT DepthStencilBuffer : public Texture {
public:
    DepthStencilBuffer() noexcept;
    DepthStencilBuffer(const DepthStencilBuffer&) = delete;
    DepthStencilBuffer& operator=(const DepthStencilBuffer&) = delete;

    virtual ~DepthStencilBuffer();

    /// Gets the width of the texture data, in pixels.
    virtual std::int32_t GetWidth() const noexcept = 0;

    /// Gets the height of the texture data, in pixels.
    virtual std::int32_t GetHeight() const noexcept = 0;

    /// Gets the format of the pixel data in the depth-stencil buffer.
    virtual SurfaceFormat GetFormat() const noexcept = 0;

    /// Gets the size of the texture resource.
    virtual Rectangle GetBounds() const noexcept = 0;
};

} // namespace Pomdog
