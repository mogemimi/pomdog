// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "SamplerStateDirect3D12.hpp"
#include "Pomdog/Graphics/SamplerDescription.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog {
namespace Detail {
namespace Direct3D12 {
namespace {

static D3D12_FILTER ToFilter(TextureFilter textureFilter) noexcept
{
    switch (textureFilter) {
    case TextureFilter::Anisotropic:    return D3D12_FILTER_ANISOTROPIC;
    case TextureFilter::Linear:         return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    case TextureFilter::Point:          return D3D12_FILTER_MIN_MAG_MIP_POINT;
    case TextureFilter::LinearMipPoint: return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    case TextureFilter::PointMipLinear: return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
    case TextureFilter::MinLinearMagPointMipLinear: return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
    case TextureFilter::MinLinearMagPointMipPoint:  return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
    case TextureFilter::MinPointMagLinearMipLinear: return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
    case TextureFilter::MinPointMagLinearMipPoint:  return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
    }
    return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
}
//-----------------------------------------------------------------------
static D3D12_TEXTURE_ADDRESS_MODE ToTextureAddressMode(TextureAddressMode addressMode) noexcept
{
    switch (addressMode) {
    case TextureAddressMode::Wrap: return D3D12_TEXTURE_ADDRESS_WRAP;
    case TextureAddressMode::Clamp: return D3D12_TEXTURE_ADDRESS_CLAMP;
    case TextureAddressMode::Mirror: return D3D12_TEXTURE_ADDRESS_MIRROR;
    }
    return D3D12_TEXTURE_ADDRESS_WRAP;
}

} // unnamed namespace
  //-----------------------------------------------------------------------
SamplerStateDirect3D12::SamplerStateDirect3D12(
    ID3D12Device* device,
    SamplerDescription const& description)
{
    POMDOG_ASSERT(device != nullptr);

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_SHADER_VISIBLE;
    heapDesc.NodeMask = 0;
    heapDesc.NumDescriptors = 1;
    heapDesc.Type = D3D12_SAMPLER_DESCRIPTOR_HEAP;

    HRESULT hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&samplerHeap));
    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create descriptor heap");
    }

    D3D12_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = ToFilter(description.Filter);
    samplerDesc.AddressU = ToTextureAddressMode(description.AddressU);
    samplerDesc.AddressV = ToTextureAddressMode(description.AddressV);
    samplerDesc.AddressW = ToTextureAddressMode(description.AddressW);
    samplerDesc.MinLOD = description.MinMipLevel;
    samplerDesc.MaxLOD = description.MaxMipLevel;
    samplerDesc.MipLODBias = description.MipMapLevelOfDetailBias;
    samplerDesc.MaxAnisotropy = description.MaxAnisotropy;

    ///@todo Add support for the following options in SamplerDescription
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_NEVER;
    samplerDesc.BorderColor[0] = 0.0f;
    samplerDesc.BorderColor[1] = 0.0f;
    samplerDesc.BorderColor[2] = 0.0f;
    samplerDesc.BorderColor[3] = 0.0f;

    POMDOG_ASSERT(samplerDesc.MinLOD <= samplerDesc.MaxLOD);
    POMDOG_ASSERT(samplerDesc.MaxLOD <= D3D12_FLOAT32_MAX);

    device->CreateSampler(&samplerDesc,
        samplerHeap->GetCPUDescriptorHandleForHeapStart());
}
//-----------------------------------------------------------------------
ID3D12DescriptorHeap* SamplerStateDirect3D12::GetDescriptorHeap()
{
    return samplerHeap.Get();
}
//-----------------------------------------------------------------------
} // namespace Direct3D12
} // namespace Detail
} // namespace Pomdog
