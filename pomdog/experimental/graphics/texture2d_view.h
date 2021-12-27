// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/graphics/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <cstdint>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

enum class Texture2DViewIndex : std::int8_t {
    Texture2D,
    RenderTarget2D,
};

class Texture2DVariant final {
public:
    std::shared_ptr<Texture> variant;
    Texture2DViewIndex index = Texture2DViewIndex::Texture2D;

public:
    [[nodiscard]] std::int32_t GetWidth() const;

    [[nodiscard]] std::int32_t GetHeight() const;

    [[nodiscard]] SurfaceFormat GetFormat() const;
};

class Texture2DView final {
private:
    Texture2DVariant body;

public:
    Texture2DView() = default;
    Texture2DView(const Texture2DView&) = default;
    Texture2DView(Texture2DView&&) = default;

    Texture2DView(std::nullptr_t);
    Texture2DView(const std::shared_ptr<Texture2D>& textureIn);
    Texture2DView(std::shared_ptr<Texture2D>&& textureIn);
    Texture2DView(std::shared_ptr<RenderTarget2D>&& renderTargetIn);
    Texture2DView(const std::shared_ptr<RenderTarget2D>& renderTargetIn);

    Texture2DView& operator=(const Texture2DView&) = default;
    Texture2DView& operator=(Texture2DView&&) = default;

    Texture2DView& operator=(const std::shared_ptr<Texture2D>& textureIn);
    Texture2DView& operator=(std::shared_ptr<Texture2D>&& textureIn);
    Texture2DView& operator=(std::shared_ptr<RenderTarget2D>&& renderTargetIn);
    Texture2DView& operator=(const std::shared_ptr<RenderTarget2D>& renderTargetIn);
    Texture2DView& operator=(std::nullptr_t);

    [[nodiscard]] bool operator==(const Texture2DView& v) const noexcept;
    [[nodiscard]] bool operator==(std::nullptr_t) const noexcept;

    [[nodiscard]] bool operator!=(const Texture2DView& v) const noexcept;
    [[nodiscard]] bool operator!=(std::nullptr_t) const noexcept;

    [[nodiscard]] const Texture2DVariant* operator->() const noexcept;

    [[nodiscard]] operator bool() const noexcept;

    [[nodiscard]] Texture2DViewIndex GetIndex() const noexcept;

    [[nodiscard]] std::shared_ptr<Texture2D> AsTexture2D() const noexcept;

    [[nodiscard]] std::shared_ptr<RenderTarget2D> AsRenderTarget2D() const noexcept;
};

} // namespace pomdog
