// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/viewport.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/math/vector4.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <memory>
#include <optional>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class Texture;
} // namespace pomdog::gpu

namespace pomdog::gpu {

/// RenderTargetAndClearColor pairs a render target texture with an optional clear color.
using RenderTargetAndClearColor = std::tuple<std::shared_ptr<Texture>, std::optional<Vector4>>;

/// RenderPass describes the render targets, depth-stencil buffer, and clear values for a single rendering pass.
struct POMDOG_EXPORT RenderPass final {
    /// An array of render targets.
    std::array<RenderTargetAndClearColor, 8> renderTargets;

    /// A depth stencil buffer.
    std::shared_ptr<Texture> depthStencilBuffer;

    /// A viewport for projection transformations and clipping.
    std::optional<Viewport> viewport;

    /// A scissor rectangle for a scissor test.
    std::optional<Rect2D> scissorRect;

    /// The depth value to use when the depth buffer is cleared.
    std::optional<f32> clearDepth;

    /// The stencil value to use when the stencil buffer is cleared.
    std::optional<u8> clearStencil;
};

} // namespace pomdog::gpu
