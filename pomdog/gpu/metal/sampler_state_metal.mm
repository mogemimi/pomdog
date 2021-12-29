// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/metal/sampler_state_metal.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/metal/metal_format_helper.h"
#include "pomdog/gpu/sampler_descriptor.h"
#include "pomdog/utility/assert.h"

namespace pomdog::gpu::detail::metal {
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
SamplerStateMetal::Initialize(id<MTLDevice> device, const SamplerDescriptor& descriptor) noexcept
{
    POMDOG_ASSERT(device != nullptr);

    MTLSamplerDescriptor* samplerDesc = [[MTLSamplerDescriptor alloc] init];
    samplerDesc.sAddressMode = ToSamplerAddressMode(descriptor.AddressU);
    samplerDesc.tAddressMode = ToSamplerAddressMode(descriptor.AddressV);
    samplerDesc.rAddressMode = ToSamplerAddressMode(descriptor.AddressW);

    switch (descriptor.Filter) {
    case TextureFilter::Anisotropic:
        samplerDesc.minFilter = MTLSamplerMinMagFilterLinear;
        samplerDesc.magFilter = MTLSamplerMinMagFilterLinear;
        samplerDesc.mipFilter = MTLSamplerMipFilterNotMipmapped;
        break;
    case TextureFilter::Linear:
        samplerDesc.minFilter = MTLSamplerMinMagFilterLinear;
        samplerDesc.magFilter = MTLSamplerMinMagFilterLinear;
        samplerDesc.mipFilter = MTLSamplerMipFilterLinear;
        break;
    case TextureFilter::Point:
        samplerDesc.minFilter = MTLSamplerMinMagFilterNearest;
        samplerDesc.magFilter = MTLSamplerMinMagFilterNearest;
        samplerDesc.mipFilter = MTLSamplerMipFilterNearest;
        break;
    case TextureFilter::LinearMipPoint:
        samplerDesc.minFilter = MTLSamplerMinMagFilterLinear;
        samplerDesc.magFilter = MTLSamplerMinMagFilterLinear;
        samplerDesc.mipFilter = MTLSamplerMipFilterNearest;
        break;
    case TextureFilter::PointMipLinear:
        samplerDesc.minFilter = MTLSamplerMinMagFilterNearest;
        samplerDesc.magFilter = MTLSamplerMinMagFilterNearest;
        samplerDesc.mipFilter = MTLSamplerMipFilterLinear;
        break;
    case TextureFilter::MinLinearMagPointMipLinear:
        samplerDesc.minFilter = MTLSamplerMinMagFilterLinear;
        samplerDesc.magFilter = MTLSamplerMinMagFilterNearest;
        samplerDesc.mipFilter = MTLSamplerMipFilterLinear;
        break;
    case TextureFilter::MinLinearMagPointMipPoint:
        samplerDesc.minFilter = MTLSamplerMinMagFilterLinear;
        samplerDesc.magFilter = MTLSamplerMinMagFilterNearest;
        samplerDesc.mipFilter = MTLSamplerMipFilterNearest;
        break;
    case TextureFilter::MinPointMagLinearMipLinear:
        samplerDesc.minFilter = MTLSamplerMinMagFilterNearest;
        samplerDesc.magFilter = MTLSamplerMinMagFilterLinear;
        samplerDesc.mipFilter = MTLSamplerMipFilterLinear;
        break;
    case TextureFilter::MinPointMagLinearMipPoint:
        samplerDesc.minFilter = MTLSamplerMinMagFilterNearest;
        samplerDesc.magFilter = MTLSamplerMinMagFilterLinear;
        samplerDesc.mipFilter = MTLSamplerMipFilterNearest;
        break;
    }

    samplerDesc.lodMaxClamp = descriptor.MaxMipLevel;
    samplerDesc.lodMinClamp = descriptor.MinMipLevel;

    samplerDesc.compareFunction = ToComparisonFunction(descriptor.ComparisonFunction);

    // NOTE: `MTLSamplerDescriptor's max anisotropy value must be >= one.
    samplerDesc.maxAnisotropy = std::max<std::uint32_t>(descriptor.MaxAnisotropy, 1);

    samplerState = [device newSamplerStateWithDescriptor:samplerDesc];
    if (samplerState == nullptr) {
        return errors::New("failed to create MTLSamplerState");
    }
    return nullptr;
}

id<MTLSamplerState> SamplerStateMetal::GetSamplerState() const noexcept
{
    return samplerState;
}

} // namespace pomdog::gpu::detail::metal
