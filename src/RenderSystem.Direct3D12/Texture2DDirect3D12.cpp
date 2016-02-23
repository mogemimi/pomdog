// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Texture2DDirect3D12.hpp"
#include "../RenderSystem.DXGI/DXGIFormatHelper.hpp"
#include "../RenderSystem/TextureHelper.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <limits>
#include <algorithm>

namespace Pomdog {
namespace Detail {
namespace Direct3D12 {
//-----------------------------------------------------------------------
Texture2DDirect3D12::Texture2DDirect3D12(
    ID3D12Device* device,
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    SurfaceFormat format)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 0);

    using DXGI::DXGIFormatHelper;
    constexpr UINT sampleCount = 1;
    constexpr UINT sampleQuality = 0;

    auto mipLevels = static_cast<UINT16>(std::min<decltype(levelCount)>(
            levelCount,
            std::numeric_limits<UINT16>::max()));
    const auto dxgiFormat = DXGIFormatHelper::ToDXGIFormat(format);
    D3D12_HEAP_PROPERTIES heapProps = CD3D12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC resourceDesc = CD3D12_RESOURCE_DESC::Tex2D(
        dxgiFormat,
        pixelWidth, pixelHeight, 1, mipLevels, sampleCount, sampleQuality,
        D3D12_RESOURCE_FLAG_NONE,
        D3D12_TEXTURE_LAYOUT_UNKNOWN);

    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&texture2D));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create texture2D");
    }

#ifdef DEBUG
    texture2D->SetName(L"Pomdog.Texture2D");
#endif

    // Create descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    heapDesc.NodeMask = 0;
    heapDesc.NumDescriptors = 1;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

    hr = device->CreateDescriptorHeap(
        &heapDesc, IID_PPV_ARGS(&shaderResourceViewHeap));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create descriptor heap");
    }

    // Create shader resource view (SRV)
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = dxgiFormat;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = mipLevels;
    srvDesc.Texture2D.PlaneSlice = 0;

    ///@todo FIXME
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.ResourceMinLODClamp = 0;

    POMDOG_ASSERT(shaderResourceViewHeap);

    device->CreateShaderResourceView(
        texture2D.Get(),
        &srvDesc,
        shaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart());
}
//-----------------------------------------------------------------------
void Texture2DDirect3D12::SetData(
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    SurfaceFormat format,
    void const* pixelData)
{
    POMDOG_ASSERT(texture2D);
    POMDOG_ASSERT(pixelData != nullptr);
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 0);

    /*
    std::size_t sizeInBytes = TextureHelper::ComputeTextureSizeInBytes(
        pixelWidth, pixelHeight, levelCount, format);

    UINT8* dataBegin = nullptr;
    HRESULT hr = texture2D->Map(0, nullptr, reinterpret_cast<void**>(&dataBegin));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to map buffer");
    }

    std::memcpy(dataBegin, pixelData, sizeInBytes);
    texture2D->Unmap(0, nullptr);
    */

    ///@todo FIXME
    UINT components = ([format] {
        switch (format) {
        case SurfaceFormat::A8_UNorm:
        case SurfaceFormat::R8_UNorm:
            return 1;
        case SurfaceFormat::B8G8R8A8_UNorm:
        case SurfaceFormat::R8G8B8A8_UNorm:
            return 4;
        default:
            break;
        }
        return 4;
    })();

    D3D12_BOX box;
    box.back = 1;
    box.right = pixelWidth;
    box.bottom = pixelHeight;
    box.left = 0;
    box.top = 0;
    box.front = 0;
    HRESULT hr = texture2D->WriteToSubresource(0, &box, pixelData, components * pixelWidth, components * pixelWidth * pixelHeight);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to WriteToSubresource");
    }
}
//-----------------------------------------------------------------------
ID3D12DescriptorHeap* Texture2DDirect3D12::GetShaderResourceViewHeap() const
{
    POMDOG_ASSERT(shaderResourceViewHeap);
    return shaderResourceViewHeap.Get();
}
//-----------------------------------------------------------------------
} // namespace Direct3D12
} // namespace Detail
} // namespace Pomdog
