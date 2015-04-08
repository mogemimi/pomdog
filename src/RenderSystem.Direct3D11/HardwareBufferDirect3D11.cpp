// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "HardwareBufferDirect3D11.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <utility>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {
namespace {

struct BufferDescription {
    D3D11_USAGE Usage;
    UINT CPUAccessFlags;
};

static BufferDescription ToDescription(BufferUsage bufferUsage)
{
    switch (bufferUsage) {
    case BufferUsage::Dynamic:
        return{ D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE };
    case BufferUsage::Immutable:
        return{ D3D11_USAGE_IMMUTABLE, 0 };
    }
    return{ D3D11_USAGE_DEFAULT, 0 };
}
//-----------------------------------------------------------------------
static ID3D11Buffer* CreateNativeBuffer(ID3D11Device* nativeDevice,
    std::size_t sizeInBytes, void const* data, BufferUsage bufferUsage,
    D3D11_BIND_FLAG bindFlag)
{
    POMDOG_ASSERT(bindFlag == D3D11_BIND_CONSTANT_BUFFER
        || bindFlag == D3D11_BIND_INDEX_BUFFER
        || bindFlag == D3D11_BIND_VERTEX_BUFFER);

#if defined(DEBUG) && !defined(NDEBUG)
    if (bindFlag == D3D11_BIND_CONSTANT_BUFFER) {
        POMDOG_ASSERT_MESSAGE(sizeInBytes % 16 == 0,
            "You must set the sizeInBytes value in multiples of 16.");
        POMDOG_ASSERT_MESSAGE(sizeInBytes <= D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT,
            "You must set the sizeInBytes value less than or equal to D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT.");
    }
#endif

    const auto description = ToDescription(bufferUsage);

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.Usage = description.Usage;
    bufferDesc.ByteWidth = static_cast<::UINT>(sizeInBytes);
    bufferDesc.BindFlags = bindFlag;
    bufferDesc.CPUAccessFlags = description.CPUAccessFlags;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA subresourceData;
    subresourceData.pSysMem = data;
    subresourceData.SysMemPitch = 0;
    subresourceData.SysMemSlicePitch = 0;

    POMDOG_ASSERT(nativeDevice);
    POMDOG_ASSERT((bufferDesc.Usage != D3D11_USAGE_IMMUTABLE)
        || ((bufferDesc.Usage == D3D11_USAGE_IMMUTABLE) && (data != nullptr)));

    auto initialData = (data != nullptr) ? &subresourceData : nullptr;

    ID3D11Buffer* buffer = nullptr;
    HRESULT hr = nativeDevice->CreateBuffer(&bufferDesc, initialData, &buffer);

    if (FAILED(hr)) {
        // error: FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create ID3D11Buffer");
    }

    return std::move(buffer);
}

} // unnamed namespace
//-----------------------------------------------------------------------
HardwareBufferDirect3D11::HardwareBufferDirect3D11(ID3D11Device* nativeDevice,
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> const& deviceContextIn,
    std::size_t sizeInBytes, BufferUsage bufferUsage, D3D11_BIND_FLAG bindFlag)
    : deviceContext(deviceContextIn)
{
    nativeBuffer = CreateNativeBuffer(nativeDevice, sizeInBytes,
        nullptr, bufferUsage, bindFlag);
}
//-----------------------------------------------------------------------
HardwareBufferDirect3D11::HardwareBufferDirect3D11(ID3D11Device* nativeDevice,
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> const& deviceContextIn,
    void const* sourceData, std::size_t sizeInBytes,
    BufferUsage bufferUsage, D3D11_BIND_FLAG bindFlag)
    : deviceContext(deviceContextIn)
{
    nativeBuffer = CreateNativeBuffer(nativeDevice, sizeInBytes,
        sourceData, bufferUsage, bindFlag);
}
//-----------------------------------------------------------------------
void HardwareBufferDirect3D11::GetData(std::size_t sizeInBytes,
    void* output) const
{
    POMDOG_ASSERT(nativeBuffer);
    POMDOG_ASSERT(deviceContext);
    POMDOG_ASSERT(output != nullptr);
    POMDOG_ASSERT(sizeInBytes > 0);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    auto hr = deviceContext->Map(nativeBuffer.Get(), 0,
        D3D11_MAP_READ, 0, &mappedResource);

    if (FAILED(hr)) {
        // error: FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to map buffer");
    }

    constexpr std::size_t offsetInBytes = 0;
    auto mappedMemory = reinterpret_cast<std::uint8_t*>(mappedResource.pData)
        + offsetInBytes;
    std::memcpy(output, mappedMemory, sizeInBytes);

    deviceContext->Unmap(nativeBuffer.Get(), 0);
}
//-----------------------------------------------------------------------
void HardwareBufferDirect3D11::SetData(std::size_t offsetInBytes,
    void const* source, std::size_t sizeInBytes)
{
    POMDOG_ASSERT(nativeBuffer);
    POMDOG_ASSERT(deviceContext);
    POMDOG_ASSERT(source != nullptr);
    POMDOG_ASSERT(sizeInBytes > 0);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    auto hr = deviceContext->Map(nativeBuffer.Get(), 0,
        D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (FAILED(hr)) {
        // error: FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to map buffer");
    }

    auto mappedMemory = reinterpret_cast<std::uint8_t*>(mappedResource.pData)
        + offsetInBytes;
    std::memcpy(mappedMemory, source, sizeInBytes);

    deviceContext->Unmap(nativeBuffer.Get(), 0);
}
//-----------------------------------------------------------------------
ID3D11Buffer* HardwareBufferDirect3D11::GetBuffer() const
{
    POMDOG_ASSERT(nativeBuffer);
    return nativeBuffer.Get();
}
//-----------------------------------------------------------------------
} // namespace Direct3D11
} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog
