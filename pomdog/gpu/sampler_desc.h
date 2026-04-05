// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/comparison_function.h"
#include "pomdog/gpu/texture_address_mode.h"
#include "pomdog/gpu/texture_filter.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

struct POMDOG_EXPORT SamplerDesc final {
    u32 maxAnisotropy = 0;
    f32 minMipLevel = 0;
    f32 maxMipLevel = std::numeric_limits<f32>::max();
    f32 mipmapLevelOfDetailBias = 0;
    TextureFilter filter = TextureFilter::Linear;
    TextureAddressMode addressU = TextureAddressMode::Clamp;
    TextureAddressMode addressV = TextureAddressMode::Clamp;
    TextureAddressMode addressW = TextureAddressMode::Clamp;
    ComparisonFunction comparisonFunction = ComparisonFunction::Never;
    //std::array<f32, 4> borderColor;

    [[nodiscard]] static SamplerDesc
    createDefault() noexcept;

    [[nodiscard]] static SamplerDesc
    createAnisotropicClamp() noexcept;

    [[nodiscard]] static SamplerDesc
    createAnisotropicWrap() noexcept;

    [[nodiscard]] static SamplerDesc
    createLinearClamp() noexcept;

    [[nodiscard]] static SamplerDesc
    createLinearWrap() noexcept;

    [[nodiscard]] static SamplerDesc
    createPointClamp() noexcept;

    [[nodiscard]] static SamplerDesc
    createPointWrap() noexcept;
};

} // namespace pomdog::gpu
