// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/sampler_descriptor.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

SamplerDescriptor SamplerDescriptor::CreateDefault() noexcept
{
    return CreateLinearClamp();
}

SamplerDescriptor SamplerDescriptor::CreateAnisotropicClamp() noexcept
{
    SamplerDescriptor desc;
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

SamplerDescriptor SamplerDescriptor::CreateAnisotropicWrap() noexcept
{
    SamplerDescriptor desc;
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

SamplerDescriptor SamplerDescriptor::CreateLinearClamp() noexcept
{
    SamplerDescriptor desc;
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

SamplerDescriptor SamplerDescriptor::CreateLinearWrap() noexcept
{
    SamplerDescriptor desc;
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

SamplerDescriptor SamplerDescriptor::CreatePointClamp() noexcept
{
    SamplerDescriptor desc;
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

SamplerDescriptor SamplerDescriptor::CreatePointWrap() noexcept
{
    SamplerDescriptor desc;
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

} // namespace pomdog
