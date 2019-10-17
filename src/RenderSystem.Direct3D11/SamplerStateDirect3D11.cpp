// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "SamplerStateDirect3D11.hpp"
#include "Pomdog/Graphics/SamplerDescription.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <array>

namespace Pomdog::Detail::Direct3D11 {
namespace {

D3D11_FILTER ToFilter(TextureFilter textureFilter) noexcept
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

D3D11_TEXTURE_ADDRESS_MODE ToTextureAddressMode(TextureAddressMode addressMode) noexcept
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

} // unnamed namespace

SamplerStateDirect3D11::SamplerStateDirect3D11(
    ID3D11Device* device,
    const SamplerDescription& description)
{
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = ToFilter(description.Filter);
    samplerDesc.AddressU = ToTextureAddressMode(description.AddressU);
    samplerDesc.AddressV = ToTextureAddressMode(description.AddressV);
    samplerDesc.AddressW = ToTextureAddressMode(description.AddressW);
    samplerDesc.MinLOD = description.MinMipLevel;
    samplerDesc.MaxLOD = description.MaxMipLevel;
    samplerDesc.MipLODBias = description.MipMapLevelOfDetailBias;
    samplerDesc.MaxAnisotropy = description.MaxAnisotropy;

    ///@todo Add support for the following options in SamplerDescription
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.BorderColor[0] = 0.0f;
    samplerDesc.BorderColor[1] = 0.0f;
    samplerDesc.BorderColor[2] = 0.0f;
    samplerDesc.BorderColor[3] = 0.0f;

    POMDOG_ASSERT(samplerDesc.MinLOD <= samplerDesc.MaxLOD);
    POMDOG_ASSERT(samplerDesc.MaxLOD <= D3D11_FLOAT32_MAX);

    POMDOG_ASSERT(device != nullptr);
    HRESULT hr = device->CreateSamplerState(&samplerDesc, &samplerState);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create ID3D11SamplerState.");
    }
}

ID3D11SamplerState* SamplerStateDirect3D11::GetSamplerState() const
{
    POMDOG_ASSERT(samplerState);
    return samplerState.Get();
}

} // namespace Pomdog::Detail::Direct3D11
