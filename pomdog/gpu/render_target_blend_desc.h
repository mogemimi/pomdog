// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/blend_factor.h"
#include "pomdog/gpu/blend_operation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

/// RenderTargetBlendDesc describes the blend state for a single render target.
struct POMDOG_EXPORT RenderTargetBlendDesc final {
    /// Blend factor applied to the source RGB color.
    BlendFactor colorSourceBlend = BlendFactor::One;

    /// Blend factor applied to the destination RGB color.
    BlendFactor colorDestinationBlend = BlendFactor::Zero;

    /// Arithmetic operation combining source and destination colors.
    BlendOperation colorBlendOperation = BlendOperation::Add;

    /// Blend factor applied to the source alpha.
    BlendFactor alphaSourceBlend = BlendFactor::One;

    /// Blend factor applied to the destination alpha.
    BlendFactor alphaDestinationBlend = BlendFactor::Zero;

    /// Arithmetic operation combining source and destination alpha.
    BlendOperation alphaBlendOperation = BlendOperation::Add;

    /// Enables blending for this render target; when false the source is written directly.
    bool blendEnable = false;

    /// Returns a blend descriptor for additive blending `(src*srcAlpha + dst*1)`.
    [[nodiscard]] static RenderTargetBlendDesc
    createAdditive() noexcept;

    /// Returns a blend descriptor for premultiplied alpha blending `(src*1 + dst*invSrcAlpha)`.
    [[nodiscard]] static RenderTargetBlendDesc
    createAlphaBlend() noexcept;

    /// Returns a blend descriptor for non-premultiplied alpha blending `(src*srcAlpha + dst*invSrcAlpha)`.
    [[nodiscard]] static RenderTargetBlendDesc
    createNonPremultiplied() noexcept;

    /// Returns a blend descriptor with blending disabled for opaque rendering.
    [[nodiscard]] static RenderTargetBlendDesc
    createOpaque() noexcept;
};

} // namespace pomdog::gpu
