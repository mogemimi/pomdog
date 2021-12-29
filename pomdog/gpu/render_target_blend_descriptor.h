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
    BlendFactor ColorSourceBlend;
    BlendFactor ColorDestinationBlend;
    BlendOperation ColorBlendOperation;
    BlendFactor AlphaSourceBlend;
    BlendFactor AlphaDestinationBlend;
    BlendOperation AlphaBlendOperation;
    bool BlendEnable;

    [[nodiscard]] static RenderTargetBlendDescriptor
    CreateAdditive() noexcept;

    [[nodiscard]] static RenderTargetBlendDescriptor
    CreateAlphaBlend() noexcept;

    [[nodiscard]] static RenderTargetBlendDescriptor
    CreateNonPremultiplied() noexcept;

    [[nodiscard]] static RenderTargetBlendDescriptor
    CreateOpaque() noexcept;
};

} // namespace pomdog::gpu
