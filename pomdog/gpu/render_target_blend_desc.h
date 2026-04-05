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

struct POMDOG_EXPORT RenderTargetBlendDesc final {
    BlendFactor colorSourceBlend = BlendFactor::One;
    BlendFactor colorDestinationBlend = BlendFactor::Zero;
    BlendOperation colorBlendOperation = BlendOperation::Add;
    BlendFactor alphaSourceBlend = BlendFactor::One;
    BlendFactor alphaDestinationBlend = BlendFactor::Zero;
    BlendOperation alphaBlendOperation = BlendOperation::Add;
    bool blendEnable = false;

    [[nodiscard]] static RenderTargetBlendDesc
    createAdditive() noexcept;

    [[nodiscard]] static RenderTargetBlendDesc
    createAlphaBlend() noexcept;

    [[nodiscard]] static RenderTargetBlendDesc
    createNonPremultiplied() noexcept;

    [[nodiscard]] static RenderTargetBlendDesc
    createOpaque() noexcept;
};

} // namespace pomdog::gpu
