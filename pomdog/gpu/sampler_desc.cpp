// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/sampler_desc.h"

namespace pomdog::gpu {

SamplerDesc SamplerDesc::createDefault() noexcept
{
    return createLinearClamp();
}

SamplerDesc SamplerDesc::createAnisotropicClamp() noexcept
{
    SamplerDesc desc = {};
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

SamplerDesc SamplerDesc::createAnisotropicWrap() noexcept
{
    SamplerDesc desc = {};
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

SamplerDesc SamplerDesc::createLinearClamp() noexcept
{
    SamplerDesc desc = {};
    desc.filter = TextureFilter::Linear;
    desc.addressU = TextureAddressMode::Clamp;
    desc.addressV = TextureAddressMode::Clamp;
    desc.addressW = TextureAddressMode::Clamp;
    desc.mipmapLevelOfDetailBias = 0;
    desc.maxAnisotropy = 0;
    desc.minMipLevel = 0;
    desc.maxMipLevel = std::numeric_limits<f32>::max();
    desc.comparisonFunction = ComparisonFunction::Never;
    return desc;
}

SamplerDesc SamplerDesc::createLinearWrap() noexcept
{
    SamplerDesc desc = {};
    desc.filter = TextureFilter::Linear;
    desc.addressU = TextureAddressMode::Wrap;
    desc.addressV = TextureAddressMode::Wrap;
    desc.addressW = TextureAddressMode::Wrap;
    desc.mipmapLevelOfDetailBias = 0;
    desc.maxAnisotropy = 0;
    desc.minMipLevel = 0;
    desc.maxMipLevel = std::numeric_limits<f32>::max();
    desc.comparisonFunction = ComparisonFunction::Never;
    return desc;
}

SamplerDesc SamplerDesc::createPointClamp() noexcept
{
    SamplerDesc desc = {};
    desc.filter = TextureFilter::Point;
    desc.addressU = TextureAddressMode::Clamp;
    desc.addressV = TextureAddressMode::Clamp;
    desc.addressW = TextureAddressMode::Clamp;
    desc.mipmapLevelOfDetailBias = 0;
    desc.maxAnisotropy = 0;
    desc.minMipLevel = 0;
    desc.maxMipLevel = std::numeric_limits<f32>::max();
    desc.comparisonFunction = ComparisonFunction::Never;
    return desc;
}

SamplerDesc SamplerDesc::createPointWrap() noexcept
{
    SamplerDesc desc = {};
    desc.filter = TextureFilter::Point;
    desc.addressU = TextureAddressMode::Wrap;
    desc.addressV = TextureAddressMode::Wrap;
    desc.addressW = TextureAddressMode::Wrap;
    desc.mipmapLevelOfDetailBias = 0;
    desc.maxAnisotropy = 0;
    desc.minMipLevel = 0;
    desc.maxMipLevel = std::numeric_limits<f32>::max();
    desc.comparisonFunction = ComparisonFunction::Never;
    return desc;
}

} // namespace pomdog::gpu
