// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/graphics/metal/sampler_state_metal.hpp"
#include "pomdog/basic/unreachable.hpp"
#include "pomdog/graphics/metal/metal_format_helper.hpp"
#include "pomdog/graphics/sampler_description.hpp"
#include "pomdog/utility/assert.hpp"

namespace Pomdog::Detail::Metal {
namespace {

MTLSamplerAddressMode ToSamplerAddressMode(TextureAddressMode addressMode) noexcept
{
    switch (addressMode) {
    case TextureAddressMode::Wrap:
        return MTLSamplerAddressModeRepeat;
    case TextureAddressMode::Clamp:
        return MTLSamplerAddressModeClampToEdge;
    case TextureAddressMode::Mirror:
        return MTLSamplerAddressModeMirrorRepeat;
    case TextureAddressMode::Border:
        return MTLSamplerAddressModeClampToZero;
    }
    POMDOG_UNREACHABLE("Unsupported texture address mode");
}

} // namespace

SamplerStateMetal::~SamplerStateMetal() = default;

std::unique_ptr<Error>
SamplerStateMetal::Initialize(id<MTLDevice> device, const SamplerDescription& description) noexcept
{
    POMDOG_ASSERT(device != nullptr);

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

    descriptor.compareFunction = ToComparisonFunction(description.ComparisonFunction);

    // NOTE: `MTLSamplerDescriptor's max anisotropy value must be >= one.
    descriptor.maxAnisotropy = std::max<std::uint32_t>(description.MaxAnisotropy, 1);

    samplerState = [device newSamplerStateWithDescriptor:descriptor];
    if (samplerState == nullptr) {
        return Errors::New("failed to create MTLSamplerState");
    }
    return nullptr;
}

id<MTLSamplerState> SamplerStateMetal::GetSamplerState() const noexcept
{
    return samplerState;
}

} // namespace Pomdog::Detail::Metal
