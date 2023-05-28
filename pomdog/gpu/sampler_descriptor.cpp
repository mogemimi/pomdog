// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/sampler_descriptor.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

SamplerDescriptor SamplerDescriptor::createDefault() noexcept
{
    return createLinearClamp();
}

SamplerDescriptor SamplerDescriptor::createAnisotropicClamp() noexcept
{
    SamplerDescriptor desc;
    desc.filter = TextureFilter::Anisotropic;
    desc.addressU = TextureAddressMode::Clamp;
    desc.addressV = TextureAddressMode::Clamp;
    desc.addressW = TextureAddressMode::Clamp;
    desc.mipmapLevelOfDetailBias = 0;
    desc.maxAnisotropy = 0;
    desc.minMipLevel = 0;
    desc.maxMipLevel = 1000;
    desc.comparisonFunction = ComparisonFunction::Never;
    return desc;
}

SamplerDescriptor SamplerDescriptor::createAnisotropicWrap() noexcept
{
    SamplerDescriptor desc;
    desc.filter = TextureFilter::Anisotropic;
    desc.addressU = TextureAddressMode::Wrap;
    desc.addressV = TextureAddressMode::Wrap;
    desc.addressW = TextureAddressMode::Wrap;
    desc.mipmapLevelOfDetailBias = 0;
    desc.maxAnisotropy = 0;
    desc.minMipLevel = 0;
    desc.maxMipLevel = 1000;
    desc.comparisonFunction = ComparisonFunction::Never;
    return desc;
}

SamplerDescriptor SamplerDescriptor::createLinearClamp() noexcept
{
    SamplerDescriptor desc;
    desc.filter = TextureFilter::Linear;
    desc.addressU = TextureAddressMode::Clamp;
    desc.addressV = TextureAddressMode::Clamp;
    desc.addressW = TextureAddressMode::Clamp;
    desc.mipmapLevelOfDetailBias = 0;
    desc.maxAnisotropy = 0;
    desc.minMipLevel = 0;
    desc.maxMipLevel = std::numeric_limits<float>::max();
    desc.comparisonFunction = ComparisonFunction::Never;
    return desc;
}

SamplerDescriptor SamplerDescriptor::createLinearWrap() noexcept
{
    SamplerDescriptor desc;
    desc.filter = TextureFilter::Linear;
    desc.addressU = TextureAddressMode::Wrap;
    desc.addressV = TextureAddressMode::Wrap;
    desc.addressW = TextureAddressMode::Wrap;
    desc.mipmapLevelOfDetailBias = 0;
    desc.maxAnisotropy = 0;
    desc.minMipLevel = 0;
    desc.maxMipLevel = std::numeric_limits<float>::max();
    desc.comparisonFunction = ComparisonFunction::Never;
    return desc;
}

SamplerDescriptor SamplerDescriptor::createPointClamp() noexcept
{
    SamplerDescriptor desc;
    desc.filter = TextureFilter::Point;
    desc.addressU = TextureAddressMode::Clamp;
    desc.addressV = TextureAddressMode::Clamp;
    desc.addressW = TextureAddressMode::Clamp;
    desc.mipmapLevelOfDetailBias = 0;
    desc.maxAnisotropy = 0;
    desc.minMipLevel = 0;
    desc.maxMipLevel = std::numeric_limits<float>::max();
    desc.comparisonFunction = ComparisonFunction::Never;
    return desc;
}

SamplerDescriptor SamplerDescriptor::createPointWrap() noexcept
{
    SamplerDescriptor desc;
    desc.filter = TextureFilter::Point;
    desc.addressU = TextureAddressMode::Wrap;
    desc.addressV = TextureAddressMode::Wrap;
    desc.addressW = TextureAddressMode::Wrap;
    desc.mipmapLevelOfDetailBias = 0;
    desc.maxAnisotropy = 0;
    desc.minMipLevel = 0;
    desc.maxMipLevel = std::numeric_limits<float>::max();
    desc.comparisonFunction = ComparisonFunction::Never;
    return desc;
}

} // namespace pomdog::gpu
