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

struct POMDOG_EXPORT BlendDesc final {
    std::array<RenderTargetBlendDesc, 8> renderTargets = {};
    bool alphaToCoverageEnable = false;
    bool independentBlendEnable = false;

    [[nodiscard]] static BlendDesc
    createDefault() noexcept;

    [[nodiscard]] static BlendDesc
    createAdditive() noexcept;

    [[nodiscard]] static BlendDesc
    createAlphaBlend() noexcept;

    [[nodiscard]] static BlendDesc
    createNonPremultiplied() noexcept;

    [[nodiscard]] static BlendDesc
    createOpaque() noexcept;
};

} // namespace pomdog::gpu
