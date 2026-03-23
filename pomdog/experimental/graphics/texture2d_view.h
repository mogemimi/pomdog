// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class RenderTarget2D;
class Texture;
class Texture2D;
enum class PixelFormat : u8;
} // namespace pomdog::gpu

namespace pomdog {

enum class Texture2DViewIndex : i8 {
    Texture2D,
    RenderTarget2D,
};

class Texture2DVariant final {
public:
    std::shared_ptr<gpu::Texture> variant;
    Texture2DViewIndex index = Texture2DViewIndex::Texture2D;

public:
    [[nodiscard]] i32
    getWidth() const;

    [[nodiscard]] i32
    getHeight() const;

    [[nodiscard]] gpu::PixelFormat
    getFormat() const;
};

class Texture2DView final {
private:
    Texture2DVariant body;

public:
    Texture2DView() = default;
    Texture2DView(const Texture2DView&) = default;
    Texture2DView(Texture2DView&&) = default;

    Texture2DView(std::nullptr_t);
    Texture2DView(const std::shared_ptr<gpu::Texture2D>& textureIn);
    Texture2DView(std::shared_ptr<gpu::Texture2D>&& textureIn);
    Texture2DView(std::shared_ptr<gpu::RenderTarget2D>&& renderTargetIn);
    Texture2DView(const std::shared_ptr<gpu::RenderTarget2D>& renderTargetIn);

    Texture2DView& operator=(const Texture2DView&) = default;
    Texture2DView& operator=(Texture2DView&&) = default;

    Texture2DView& operator=(const std::shared_ptr<gpu::Texture2D>& textureIn);
    Texture2DView& operator=(std::shared_ptr<gpu::Texture2D>&& textureIn);
    Texture2DView& operator=(std::shared_ptr<gpu::RenderTarget2D>&& renderTargetIn);
    Texture2DView& operator=(const std::shared_ptr<gpu::RenderTarget2D>& renderTargetIn);
    Texture2DView& operator=(std::nullptr_t);

    [[nodiscard]] bool operator==(const Texture2DView& v) const noexcept;
    [[nodiscard]] bool operator==(std::nullptr_t) const noexcept;

    [[nodiscard]] bool operator!=(const Texture2DView& v) const noexcept;
    [[nodiscard]] bool operator!=(std::nullptr_t) const noexcept;

    [[nodiscard]] const Texture2DVariant* operator->() const noexcept;

    [[nodiscard]] operator bool() const noexcept;

    [[nodiscard]] Texture2DViewIndex
    getIndex() const noexcept;

    [[nodiscard]] std::shared_ptr<gpu::Texture2D>
    asTexture2D() const noexcept;

    [[nodiscard]] std::shared_ptr<gpu::RenderTarget2D>
    asRenderTarget2D() const noexcept;
};

} // namespace pomdog
