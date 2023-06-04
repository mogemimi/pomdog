// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/direct3d11/sampler_state_direct3d11.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/direct3d11/format_helper.h"
#include "pomdog/gpu/sampler_descriptor.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::direct3d11 {
namespace {

[[nodiscard]] D3D11_FILTER
toFilter(TextureFilter textureFilter) noexcept
{
    switch (textureFilter) {
    case TextureFilter::Anisotropic:
        return D3D11_FILTER_ANISOTROPIC;
    case TextureFilter::Linear:
        return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    case TextureFilter::Point:
        return D3D11_FILTER_MIN_MAG_MIP_POINT;
    case TextureFilter::LinearMipPoint:
        return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    case TextureFilter::PointMipLinear:
        return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
    case TextureFilter::MinLinearMagPointMipLinear:
        return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
    case TextureFilter::MinLinearMagPointMipPoint:
        return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
    case TextureFilter::MinPointMagLinearMipLinear:
        return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
    case TextureFilter::MinPointMagLinearMipPoint:
        return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
    }
    return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
}

[[nodiscard]] D3D11_TEXTURE_ADDRESS_MODE
toTextureAddressMode(TextureAddressMode addressMode) noexcept
{
    switch (addressMode) {
    case TextureAddressMode::Wrap:
        return D3D11_TEXTURE_ADDRESS_WRAP;
    case TextureAddressMode::Clamp:
        return D3D11_TEXTURE_ADDRESS_CLAMP;
    case TextureAddressMode::Mirror:
        return D3D11_TEXTURE_ADDRESS_MIRROR;
    case TextureAddressMode::Border:
        return D3D11_TEXTURE_ADDRESS_BORDER;
    }
    return D3D11_TEXTURE_ADDRESS_WRAP;
}

} // namespace

std::unique_ptr<Error>
SamplerStateDirect3D11::initialize(
    ID3D11Device* device,
    const SamplerDescriptor& descriptor) noexcept
{
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = toFilter(descriptor.filter);
    samplerDesc.AddressU = toTextureAddressMode(descriptor.addressU);
    samplerDesc.AddressV = toTextureAddressMode(descriptor.addressV);
    samplerDesc.AddressW = toTextureAddressMode(descriptor.addressW);
    samplerDesc.MinLOD = descriptor.minMipLevel;
    samplerDesc.MaxLOD = descriptor.maxMipLevel;
    samplerDesc.MipLODBias = descriptor.mipmapLevelOfDetailBias;
    samplerDesc.MaxAnisotropy = descriptor.maxAnisotropy;
    samplerDesc.ComparisonFunc = toComparisonFunction(descriptor.comparisonFunction);

    // TODO: Add support for the following options in SamplerDescriptor
    samplerDesc.BorderColor[0] = 0.0f;
    samplerDesc.BorderColor[1] = 0.0f;
    samplerDesc.BorderColor[2] = 0.0f;
    samplerDesc.BorderColor[3] = 0.0f;

    POMDOG_ASSERT(samplerDesc.MinLOD <= samplerDesc.MaxLOD);
    POMDOG_ASSERT(samplerDesc.MaxLOD <= D3D11_FLOAT32_MAX);

    POMDOG_ASSERT(device != nullptr);
    if (auto hr = device->CreateSamplerState(&samplerDesc, &samplerState_); FAILED(hr)) {
        return errors::make("CreateSamplerState() failed");
    }

    return nullptr;
}

ID3D11SamplerState*
SamplerStateDirect3D11::getSamplerState() const noexcept
{
    POMDOG_ASSERT(samplerState_);
    return samplerState_.Get();
}

} // namespace pomdog::gpu::detail::direct3d11
