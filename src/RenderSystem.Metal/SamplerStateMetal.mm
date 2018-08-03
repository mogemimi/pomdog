// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "SamplerStateMetal.hpp"
#include "../Basic/Unreachable.hpp"
#include "Pomdog/Graphics/SamplerDescription.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog {
namespace Detail {
namespace Metal {
namespace {

MTLSamplerAddressMode ToSamplerAddressMode(TextureAddressMode addressMode) noexcept
{
    switch (addressMode) {
    case TextureAddressMode::Wrap: return MTLSamplerAddressModeRepeat;
    case TextureAddressMode::Clamp: return MTLSamplerAddressModeClampToEdge;
    case TextureAddressMode::Mirror: return MTLSamplerAddressModeMirrorRepeat;
    }
    POMDOG_UNREACHABLE("Unsupported texture address mode");
}

} // unnamed namespace

SamplerStateMetal::SamplerStateMetal(id<MTLDevice> device, const SamplerDescription& description)
    : samplerState(nil)
{
    POMDOG_ASSERT(device != nil);

    MTLSamplerDescriptor* descriptor = [[MTLSamplerDescriptor alloc] init];
    descriptor.sAddressMode = ToSamplerAddressMode(description.AddressU);
    descriptor.tAddressMode = ToSamplerAddressMode(description.AddressV);
    descriptor.rAddressMode = ToSamplerAddressMode(description.AddressW);

    switch (description.Filter) {
    case TextureFilter::Anisotropic:
        descriptor.minFilter = MTLSamplerMinMagFilterLinear;
        descriptor.magFilter = MTLSamplerMinMagFilterLinear;
        descriptor.mipFilter = MTLSamplerMipFilterNotMipmapped;
        break;
    case TextureFilter::Linear:
        descriptor.minFilter = MTLSamplerMinMagFilterLinear;
        descriptor.magFilter = MTLSamplerMinMagFilterLinear;
        descriptor.mipFilter = MTLSamplerMipFilterLinear;
        break;
    case TextureFilter::Point:
        descriptor.minFilter = MTLSamplerMinMagFilterNearest;
        descriptor.magFilter = MTLSamplerMinMagFilterNearest;
        descriptor.mipFilter = MTLSamplerMipFilterNearest;
        break;
    case TextureFilter::LinearMipPoint:
        descriptor.minFilter = MTLSamplerMinMagFilterLinear;
        descriptor.magFilter = MTLSamplerMinMagFilterLinear;
        descriptor.mipFilter = MTLSamplerMipFilterNearest;
        break;
    case TextureFilter::PointMipLinear:
        descriptor.minFilter = MTLSamplerMinMagFilterNearest;
        descriptor.magFilter = MTLSamplerMinMagFilterNearest;
        descriptor.mipFilter = MTLSamplerMipFilterLinear;
        break;
    case TextureFilter::MinLinearMagPointMipLinear:
        descriptor.minFilter = MTLSamplerMinMagFilterLinear;
        descriptor.magFilter = MTLSamplerMinMagFilterNearest;
        descriptor.mipFilter = MTLSamplerMipFilterLinear;
        break;
    case TextureFilter::MinLinearMagPointMipPoint:
        descriptor.minFilter = MTLSamplerMinMagFilterLinear;
        descriptor.magFilter = MTLSamplerMinMagFilterNearest;
        descriptor.mipFilter = MTLSamplerMipFilterNearest;
        break;
    case TextureFilter::MinPointMagLinearMipLinear:
        descriptor.minFilter = MTLSamplerMinMagFilterNearest;
        descriptor.magFilter = MTLSamplerMinMagFilterLinear;
        descriptor.mipFilter = MTLSamplerMipFilterLinear;
        break;
    case TextureFilter::MinPointMagLinearMipPoint:
        descriptor.minFilter = MTLSamplerMinMagFilterNearest;
        descriptor.magFilter = MTLSamplerMinMagFilterLinear;
        descriptor.mipFilter = MTLSamplerMipFilterNearest;
        break;
    }

    descriptor.lodMaxClamp = description.MaxMipLevel;
    descriptor.lodMinClamp = description.MinMipLevel;

    // NOTE: `MTLSamplerDescriptor's max anisotropy value must be >= one.
    descriptor.maxAnisotropy = std::max<std::uint32_t>(description.MaxAnisotropy, 1);

    samplerState = [device newSamplerStateWithDescriptor:descriptor];
    if (samplerState == nil) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create MTLSamplerState.");
    }
}

id<MTLSamplerState> SamplerStateMetal::GetSamplerState() const noexcept
{
    return samplerState;
}

} // namespace Metal
} // namespace Detail
} // namespace Pomdog
