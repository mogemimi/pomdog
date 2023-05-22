// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/viewport.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/math/vector4.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

using RenderTargetAndClearColor = std::tuple<std::shared_ptr<RenderTarget2D>, std::optional<Vector4>>;

struct POMDOG_EXPORT RenderPass final {
    /// An array of render targets.
    std::array<RenderTargetAndClearColor, 8> renderTargets;

    /// A depth stencil buffer.
    std::shared_ptr<DepthStencilBuffer> depthStencilBuffer;

    /// A viewport for projection transformations and clipping.
    std::optional<Viewport> viewport;

    /// A scissor rectangle for a scissor test.
    std::optional<Rectangle> scissorRect;

    /// The depth value to use when the depth buffer is cleared.
    std::optional<float> clearDepth;

    /// The stencil value to use when the stencil buffer is cleared.
    std::optional<std::uint8_t> clearStencil;
};

} // namespace pomdog::gpu
