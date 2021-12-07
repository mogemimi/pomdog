// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/graphics/comparison_function.hpp"
#include "pomdog/graphics/texture_address_mode.hpp"
#include "pomdog/graphics/texture_filter.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

struct POMDOG_EXPORT SamplerDescription final {
    std::uint32_t MaxAnisotropy;
    float MinMipLevel;
    float MaxMipLevel;
    float MipMapLevelOfDetailBias;
    TextureFilter Filter;
    TextureAddressMode AddressU;
    TextureAddressMode AddressV;
    TextureAddressMode AddressW;
    Pomdog::ComparisonFunction ComparisonFunction;
    //std::array<float, 4> BorderColor;

    static SamplerDescription CreateDefault() noexcept
    {
        return CreateLinearClamp();
    }

    static SamplerDescription CreateAnisotropicClamp() noexcept
    {
        SamplerDescription desc;
        desc.Filter = TextureFilter::Anisotropic;
        desc.AddressU = TextureAddressMode::Clamp;
        desc.AddressV = TextureAddressMode::Clamp;
        desc.AddressW = TextureAddressMode::Clamp;
        desc.MipMapLevelOfDetailBias = 0;
        desc.MaxAnisotropy = 0;
        desc.MinMipLevel = 0;
        desc.MaxMipLevel = 1000;
        desc.ComparisonFunction = ComparisonFunction::Never;
        return desc;
    }

    static SamplerDescription CreateAnisotropicWrap() noexcept
    {
        SamplerDescription desc;
        desc.Filter = TextureFilter::Anisotropic;
        desc.AddressU = TextureAddressMode::Wrap;
        desc.AddressV = TextureAddressMode::Wrap;
        desc.AddressW = TextureAddressMode::Wrap;
        desc.MipMapLevelOfDetailBias = 0;
        desc.MaxAnisotropy = 0;
        desc.MinMipLevel = 0;
        desc.MaxMipLevel = 1000;
        desc.ComparisonFunction = ComparisonFunction::Never;
        return desc;
    }

    static SamplerDescription CreateLinearClamp() noexcept
    {
        SamplerDescription desc;
        desc.Filter = TextureFilter::Linear;
        desc.AddressU = TextureAddressMode::Clamp;
        desc.AddressV = TextureAddressMode::Clamp;
        desc.AddressW = TextureAddressMode::Clamp;
        desc.MipMapLevelOfDetailBias = 0;
        desc.MaxAnisotropy = 0;
        desc.MinMipLevel = 0;
        desc.MaxMipLevel = std::numeric_limits<float>::max();
        desc.ComparisonFunction = ComparisonFunction::Never;
        return desc;
    }

    static SamplerDescription CreateLinearWrap() noexcept
    {
        SamplerDescription desc;
        desc.Filter = TextureFilter::Linear;
        desc.AddressU = TextureAddressMode::Wrap;
        desc.AddressV = TextureAddressMode::Wrap;
        desc.AddressW = TextureAddressMode::Wrap;
        desc.MipMapLevelOfDetailBias = 0;
        desc.MaxAnisotropy = 0;
        desc.MinMipLevel = 0;
        desc.MaxMipLevel = std::numeric_limits<float>::max();
        desc.ComparisonFunction = ComparisonFunction::Never;
        return desc;
    }

    static SamplerDescription CreatePointClamp() noexcept
    {
        SamplerDescription desc;
        desc.Filter = TextureFilter::Point;
        desc.AddressU = TextureAddressMode::Clamp;
        desc.AddressV = TextureAddressMode::Clamp;
        desc.AddressW = TextureAddressMode::Clamp;
        desc.MipMapLevelOfDetailBias = 0;
        desc.MaxAnisotropy = 0;
        desc.MinMipLevel = 0;
        desc.MaxMipLevel = std::numeric_limits<float>::max();
        desc.ComparisonFunction = ComparisonFunction::Never;
        return desc;
    }

    static SamplerDescription CreatePointWrap() noexcept
    {
        SamplerDescription desc;
        desc.Filter = TextureFilter::Point;
        desc.AddressU = TextureAddressMode::Wrap;
        desc.AddressV = TextureAddressMode::Wrap;
        desc.AddressW = TextureAddressMode::Wrap;
        desc.MipMapLevelOfDetailBias = 0;
        desc.MaxAnisotropy = 0;
        desc.MinMipLevel = 0;
        desc.MaxMipLevel = std::numeric_limits<float>::max();
        desc.ComparisonFunction = ComparisonFunction::Never;
        return desc;
    }
};

} // namespace Pomdog
