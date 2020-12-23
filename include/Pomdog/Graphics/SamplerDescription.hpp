// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/ComparisonFunction.hpp"
#include "Pomdog/Graphics/TextureAddressMode.hpp"
#include "Pomdog/Graphics/TextureFilter.hpp"
#include <cstdint>
#include <limits>
#include <utility>

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
