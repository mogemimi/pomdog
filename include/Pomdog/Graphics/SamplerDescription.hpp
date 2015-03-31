// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SAMPLERDESCRIPTION_CA2C90E4_HPP
#define POMDOG_SAMPLERDESCRIPTION_CA2C90E4_HPP

#include "TextureAddressMode.hpp"
#include "TextureFilter.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <limits>
#include <utility>

namespace Pomdog {

class POMDOG_EXPORT SamplerDescription {
public:
    std::uint32_t MaxAnisotropy;
    float MinMipLevel;
    float MaxMipLevel;
    float MipMapLevelOfDetailBias;
    TextureFilter Filter;
    TextureAddressMode AddressU;
    TextureAddressMode AddressV;
    TextureAddressMode AddressW;
    //std::array<float, 4> BorderColor;

public:
    static SamplerDescription CreateDefault()
    {
        return CreateLinearClamp();
    }

    static SamplerDescription CreateAnisotropicClamp()
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
        return std::move(desc);
    }

    static SamplerDescription CreateAnisotropicWrap()
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
        return std::move(desc);
    }

    static SamplerDescription CreateLinearClamp()
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
        return std::move(desc);
    }

    static SamplerDescription CreateLinearWrap()
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
        return std::move(desc);
    }

    static SamplerDescription CreatePointClamp()
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
        return std::move(desc);
    }

    static SamplerDescription CreatePointWrap()
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
        return std::move(desc);
    }
};

} // namespace Pomdog

#endif // POMDOG_SAMPLERDESCRIPTION_CA2C90E4_HPP
