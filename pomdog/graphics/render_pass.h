// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/graphics/forward_declarations.h"
#include "pomdog/graphics/viewport.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/math/vector4.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

using RenderTargetAndClearColor = std::tuple<std::shared_ptr<RenderTarget2D>, std::optional<Vector4>>;

struct POMDOG_EXPORT RenderPass final {
    /// An array of render targets.
    std::array<RenderTargetAndClearColor, 8> RenderTargets;

    /// A depth stencil buffer.
    std::shared_ptr<pomdog::DepthStencilBuffer> DepthStencilBuffer;

    /// A viewport for projection transformations and clipping.
    std::optional<pomdog::Viewport> Viewport;

    /// A scissor rectangle for a scissor test.
    std::optional<Rectangle> ScissorRect;

    /// The depth value to use when the depth buffer is cleared.
    std::optional<float> ClearDepth;

    /// The stencil value to use when the stencil buffer is cleared.
    std::optional<std::uint8_t> ClearStencil;
};

} // namespace pomdog
