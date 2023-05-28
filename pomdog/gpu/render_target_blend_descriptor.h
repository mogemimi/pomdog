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

struct POMDOG_EXPORT RenderTargetBlendDescriptor final {
    BlendFactor colorSourceBlend;
    BlendFactor colorDestinationBlend;
    BlendOperation colorBlendOperation;
    BlendFactor alphaSourceBlend;
    BlendFactor alphaDestinationBlend;
    BlendOperation alphaBlendOperation;
    bool blendEnable;

    [[nodiscard]] static RenderTargetBlendDescriptor
    createAdditive() noexcept;

    [[nodiscard]] static RenderTargetBlendDescriptor
    createAlphaBlend() noexcept;

    [[nodiscard]] static RenderTargetBlendDescriptor
    createNonPremultiplied() noexcept;

    [[nodiscard]] static RenderTargetBlendDescriptor
    createOpaque() noexcept;
};

} // namespace pomdog::gpu
