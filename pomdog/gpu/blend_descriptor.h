// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/render_target_blend_descriptor.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

struct POMDOG_EXPORT BlendDescriptor final {
    std::array<RenderTargetBlendDescriptor, 8> renderTargets;
    bool alphaToCoverageEnable;
    bool independentBlendEnable;

    [[nodiscard]] static BlendDescriptor
    createDefault() noexcept;

    [[nodiscard]] static BlendDescriptor
    createAdditive() noexcept;

    [[nodiscard]] static BlendDescriptor
    createAlphaBlend() noexcept;

    [[nodiscard]] static BlendDescriptor
    createNonPremultiplied() noexcept;

    [[nodiscard]] static BlendDescriptor
    createOpaque() noexcept;
};

} // namespace pomdog::gpu
