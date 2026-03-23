// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/comparison_function.h"
#include "pomdog/gpu/texture_address_mode.h"
#include "pomdog/gpu/texture_filter.h"

namespace pomdog::gpu {

struct POMDOG_EXPORT SamplerDesc final {
    u32 maxAnisotropy;
    f32 minMipLevel;
    f32 maxMipLevel;
    f32 mipmapLevelOfDetailBias;
    TextureFilter filter;
    TextureAddressMode addressU;
    TextureAddressMode addressV;
    TextureAddressMode addressW;
    ComparisonFunction comparisonFunction;
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
