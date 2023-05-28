// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/comparison_function.h"
#include "pomdog/gpu/texture_address_mode.h"
#include "pomdog/gpu/texture_filter.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

struct POMDOG_EXPORT SamplerDescriptor final {
    std::uint32_t maxAnisotropy;
    float minMipLevel;
    float maxMipLevel;
    float mipmapLevelOfDetailBias;
    TextureFilter filter;
    TextureAddressMode addressU;
    TextureAddressMode addressV;
    TextureAddressMode addressW;
    pomdog::gpu::ComparisonFunction comparisonFunction;
    //std::array<float, 4> borderColor;

    [[nodiscard]] static SamplerDescriptor
    createDefault() noexcept;

    [[nodiscard]] static SamplerDescriptor
    createAnisotropicClamp() noexcept;

    [[nodiscard]] static SamplerDescriptor
    createAnisotropicWrap() noexcept;

    [[nodiscard]] static SamplerDescriptor
    createLinearClamp() noexcept;

    [[nodiscard]] static SamplerDescriptor
    createLinearWrap() noexcept;

    [[nodiscard]] static SamplerDescriptor
    createPointClamp() noexcept;

    [[nodiscard]] static SamplerDescriptor
    createPointWrap() noexcept;
};

} // namespace pomdog::gpu
