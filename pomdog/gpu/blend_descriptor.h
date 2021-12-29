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
    std::array<RenderTargetBlendDescriptor, 8> RenderTargets;
    bool AlphaToCoverageEnable;
    bool IndependentBlendEnable;

    [[nodiscard]] static BlendDescriptor
    CreateDefault() noexcept;

    [[nodiscard]] static BlendDescriptor
    CreateAdditive() noexcept;

    [[nodiscard]] static BlendDescriptor
    CreateAlphaBlend() noexcept;

    [[nodiscard]] static BlendDescriptor
    CreateNonPremultiplied() noexcept;

    [[nodiscard]] static BlendDescriptor
    CreateOpaque() noexcept;
};

} // namespace pomdog::gpu
