// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/render_target_blend_desc.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

/// BlendDesc describes the blend state for all render targets in the pipeline.
struct POMDOG_EXPORT BlendDesc final {
    /// Per-render-target blend descriptors; up to 8 render targets.
    std::array<RenderTargetBlendDesc, 8> renderTargets = {};

    /// Enables alpha-to-coverage as a multisampling technique when true; requires MSAA.
    bool alphaToCoverageEnable = false;

    /// Enables independent blend states per render target when true.
    /// When false, only `renderTargets[0]` is applied to all targets.
    bool independentBlendEnable = false;

    /// Returns a blend state equivalent to `createOpaque()`.
    [[nodiscard]] static BlendDesc
    createDefault() noexcept;

    /// Returns a blend state for additive blending `(src*srcAlpha + dst*1)`.
    [[nodiscard]] static BlendDesc
    createAdditive() noexcept;

    /// Returns a blend state for premultiplied alpha blending `(src*1 + dst*invSrcAlpha)`.
    [[nodiscard]] static BlendDesc
    createAlphaBlend() noexcept;

    /// Returns a blend state for non-premultiplied alpha blending `(src*srcAlpha + dst*invSrcAlpha)`.
    [[nodiscard]] static BlendDesc
    createNonPremultiplied() noexcept;

    /// Returns a blend state with blending disabled for opaque rendering.
    [[nodiscard]] static BlendDesc
    createOpaque() noexcept;
};

} // namespace pomdog::gpu
