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
    CreateDefault() noexcept;

    [[nodiscard]] static SamplerDescriptor
    CreateAnisotropicClamp() noexcept;

    [[nodiscard]] static SamplerDescriptor
    CreateAnisotropicWrap() noexcept;

    [[nodiscard]] static SamplerDescriptor
    CreateLinearClamp() noexcept;

    [[nodiscard]] static SamplerDescriptor
    CreateLinearWrap() noexcept;

    [[nodiscard]] static SamplerDescriptor
    CreatePointClamp() noexcept;

    [[nodiscard]] static SamplerDescriptor
    CreatePointWrap() noexcept;
};

} // namespace pomdog::gpu
