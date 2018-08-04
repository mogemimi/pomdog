// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "BufferDirect3D11.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <utility>

namespace Pomdog {
namespace Detail {
namespace Direct3D11 {
namespace {

ID3D11Buffer* CreateNativeBuffer(
    ID3D11Device* device,
    std::size_t sizeInBytes,
    const void* data,
    BufferUsage bufferUsage,
    D3D11_BIND_FLAG bindFlag)
{
    POMDOG_ASSERT(bindFlag == D3D11_BIND_CONSTANT_BUFFER
        || bindFlag == D3D11_BIND_INDEX_BUFFER
        || bindFlag == D3D11_BIND_VERTEX_BUFFER);

    if (bindFlag == D3D11_BIND_CONSTANT_BUFFER) {
        POMDOG_ASSERT_MESSAGE(sizeInBytes <= D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT,
            "You must set the sizeInBytes value less than or"
            " equal to D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT.");

        if (sizeInBytes % 16 != 0) {
#if defined(DEBUG) && !defined(NDEBUG)
            Log::Warning("Pomdog",
                "You must set the sizeInBytes value in multiples of 16.\n"
                "ConstantBuffer size also needs to be a multiples of 16.");
#endif
            auto offset = sizeInBytes % 16;
            POMDOG_ASSERT(offset < 16);
            sizeInBytes += (16 - offset);
        }
    }

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = static_cast<::UINT>(sizeInBytes);
    bufferDesc.BindFlags = bindFlag;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    switch (bufferUsage) {
    case BufferUsage::Dynamic:
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        break;
    case BufferUsage::Immutable:
        bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        bufferDesc.CPUAccessFlags = 0;
        break;
    default:
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.CPUAccessFlags = 0;
        break;
    }

    D3D11_SUBRESOURCE_DATA subresourceData;
    subresourceData.pSysMem = data;
    subresourceData.SysMemPitch = 0;
    subresourceData.SysMemSlicePitch = 0;

    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT((bufferDesc.Usage != D3D11_USAGE_IMMUTABLE)
        || ((bufferDesc.Usage == D3D11_USAGE_IMMUTABLE) && (data != nullptr)));

    auto initialData = (data != nullptr) ? &subresourceData : nullptr;

    ID3D11Buffer* buffer = nullptr;
    HRESULT hr = device->CreateBuffer(&bufferDesc, initialData, &buffer);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create ID3D11Buffer");
    }

    return std::move(buffer);
}

D3D11_MAP GetMapTypeForWriting(D3D11_BIND_FLAG bindFlag) noexcept
{
    if (bindFlag == D3D11_BIND_CONSTANT_BUFFER) {
        return D3D11_MAP_WRITE_DISCARD;
    }

    // NOTE:
    // Basically, `D3D11_MAP_WRITE_NO_OVERWRITE` cannot be used on
    // a resource created with the `D3D11_BIND_CONSTANT_BUFFER` flag.
    // Please see https://msdn.microsoft.com/en-us/library/ff476181(v=vs.85).aspx
    //
    // However, Direct3D 11.1 runtime enables mapping dynamic constant buffers
    // with D3D11_MAP_WRITE_NO_OVERWRITE.
    // For more information, please see this option,
    // D3D11_FEATURE_DATA_D3D11_OPTIONS::MapNoOverwriteOnDynamicConstantBuffer
    // https://msdn.microsoft.com/en-us/library/windows/desktop/hh404457(v=vs.85).aspx
    POMDOG_ASSERT(
        bindFlag == D3D11_BIND_INDEX_BUFFER ||
        bindFlag == D3D11_BIND_VERTEX_BUFFER);
    return D3D11_MAP_WRITE_NO_OVERWRITE;
}

} // unnamed namespace

BufferDirect3D11::BufferDirect3D11(
    ID3D11Device* device,
    const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContextIn,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage,
    D3D11_BIND_FLAG bindFlag)
    : deviceContext(deviceContextIn)
    , mapTypeForWriting(GetMapTypeForWriting(bindFlag))
{
    buffer = CreateNativeBuffer(device, sizeInBytes,
        nullptr, bufferUsage, bindFlag);
}

BufferDirect3D11::BufferDirect3D11(
    ID3D11Device* device,
    const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContextIn,
    const void* sourceData,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage,
    D3D11_BIND_FLAG bindFlag)
    : deviceContext(deviceContextIn)
    , mapTypeForWriting(GetMapTypeForWriting(bindFlag))
{
    buffer = CreateNativeBuffer(device, sizeInBytes,
        sourceData, bufferUsage, bindFlag);
}

void BufferDirect3D11::GetData(
    std::size_t offsetInBytes,
    void* destination,
    std::size_t sizeInBytes) const
{
    POMDOG_ASSERT(buffer);
    POMDOG_ASSERT(deviceContext);
    POMDOG_ASSERT(destination != nullptr);
    POMDOG_ASSERT(sizeInBytes > 0);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    auto hr = deviceContext->Map(buffer.Get(), 0,
        D3D11_MAP_READ, 0, &mappedResource);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to map buffer");
    }

    auto mappedMemory = reinterpret_cast<std::uint8_t*>(mappedResource.pData)
        + offsetInBytes;
    std::memcpy(destination, mappedMemory, sizeInBytes);

    deviceContext->Unmap(buffer.Get(), 0);
}

void BufferDirect3D11::SetData(
    std::size_t offsetInBytes,
    const void* source,
    std::size_t sizeInBytes)
{
    POMDOG_ASSERT(buffer);
    POMDOG_ASSERT(deviceContext);
    POMDOG_ASSERT(source != nullptr);
    POMDOG_ASSERT(sizeInBytes > 0);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    auto hr = deviceContext->Map(buffer.Get(), 0,
        mapTypeForWriting, 0, &mappedResource);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to map buffer");
    }

    auto mappedMemory = reinterpret_cast<std::uint8_t*>(mappedResource.pData)
        + offsetInBytes;
    std::memcpy(mappedMemory, source, sizeInBytes);

    deviceContext->Unmap(buffer.Get(), 0);
}

ID3D11Buffer* BufferDirect3D11::GetBuffer() const
{
    POMDOG_ASSERT(buffer);
    return buffer.Get();
}

} // namespace Direct3D11
} // namespace Detail
} // namespace Pomdog
