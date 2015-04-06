// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "BufferDirect3D12.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <utility>

namespace Pomdog {
namespace Detail {
namespace Direct3D12 {
//-----------------------------------------------------------------------
BufferDirect3D12::BufferDirect3D12(
    ID3D12Device* device,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage)
{
    POMDOG_ASSERT(device != nullptr);

    // FIXME
    UNREFERENCED_PARAMETER(bufferUsage);

    auto heapProps = CD3D12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto desc = CD3D12_RESOURCE_DESC::Buffer(sizeInBytes);

    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_MISC_NONE,
        &desc,
        D3D12_RESOURCE_USAGE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&buffer));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create buffer");
    }

#ifdef DEBUG
    buffer->SetName(L"Pomdog.Buffer");
#endif
}
//-----------------------------------------------------------------------
BufferDirect3D12::BufferDirect3D12(
    ID3D12Device* device,
    void const* sourceData,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage)
{
    POMDOG_ASSERT(device != nullptr);

    // FIXME
    UNREFERENCED_PARAMETER(bufferUsage);

    auto heapProps = CD3D12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto desc = CD3D12_RESOURCE_DESC::Buffer(sizeInBytes);

    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_MISC_NONE,
        &desc,
        D3D12_RESOURCE_USAGE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&buffer));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create buffer");
    }

    UINT8* dataBegin = nullptr;
    hr = buffer->Map(0, nullptr, reinterpret_cast<void**>(&dataBegin));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to map buffer");
    }

    memcpy(dataBegin, sourceData, sizeInBytes);
    buffer->Unmap(0, nullptr);
}
//-----------------------------------------------------------------------
void BufferDirect3D12::GetData(std::size_t offsetInBytes,
    void* destination, std::size_t sizeInBytes) const
{
    POMDOG_ASSERT(buffer);
    POMDOG_ASSERT(destination != nullptr);
    POMDOG_ASSERT(sizeInBytes > 0);

    UINT8* dataBegin = nullptr;
    HRESULT hr = buffer->Map(0, nullptr, reinterpret_cast<void**>(&dataBegin));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to map buffer");
    }

    std::memcpy(destination, dataBegin + offsetInBytes, sizeInBytes);
    buffer->Unmap(0, nullptr);
}
//-----------------------------------------------------------------------
void BufferDirect3D12::SetData(std::size_t offsetInBytes,
    void const* source, std::size_t sizeInBytes)
{
    POMDOG_ASSERT(buffer);
    POMDOG_ASSERT(source != nullptr);
    POMDOG_ASSERT(sizeInBytes > 0);

    UINT8* dataBegin = nullptr;
    HRESULT hr = buffer->Map(0, nullptr, reinterpret_cast<void**>(&dataBegin));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to map buffer");
    }

    std::memcpy(dataBegin + offsetInBytes, source, sizeInBytes);
    buffer->Unmap(0, nullptr);
}
//-----------------------------------------------------------------------
D3D12_GPU_VIRTUAL_ADDRESS BufferDirect3D12::GetGPUVirtualAddress() const
{
    POMDOG_ASSERT(buffer);
    POMDOG_ASSERT(buffer->GetGPUVirtualAddress() != NULL);
    return buffer->GetGPUVirtualAddress();
}
//-----------------------------------------------------------------------
} // namespace Direct3D12
} // namespace Detail
} // namespace Pomdog
