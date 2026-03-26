// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/direct3d11/buffer_direct3d11.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/buffer_bind_flags.h"
#include "pomdog/gpu/buffer_desc.h"
#include "pomdog/gpu/memory_usage.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/exception.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using Microsoft::WRL::ComPtr;

namespace pomdog::gpu::detail::direct3d11 {

void BufferDirect3D11::getData(
    u32 offsetInBytes,
    void* destination,
    u32 sizeInBytes) const
{
    POMDOG_ASSERT(buffer_);
    POMDOG_ASSERT(destination != nullptr);
    POMDOG_ASSERT(sizeInBytes > 0);

    // NOTE: Get the device context
    ComPtr<ID3D11Device> device;
    buffer_->GetDevice(&device);
    ComPtr<ID3D11DeviceContext> deviceContext;
    device->GetImmediateContext(&deviceContext);

    POMDOG_ASSERT(deviceContext != nullptr);

    // NOTE: Map the buffer
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    auto hr = deviceContext->Map(
        buffer_.Get(),
        0,
        D3D11_MAP_READ,
        0,
        &mappedResource);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to map buffer");
    }

    auto mappedMemory = reinterpret_cast<u8*>(mappedResource.pData) + offsetInBytes;
    std::memcpy(destination, mappedMemory, sizeInBytes);

    deviceContext->Unmap(buffer_.Get(), 0);
}

void BufferDirect3D11::setData(
    u32 offsetInBytes,
    const void* source,
    u32 sizeInBytes)
{
    POMDOG_ASSERT(buffer_);
    POMDOG_ASSERT(source != nullptr);
    POMDOG_ASSERT(sizeInBytes > 0);

    // NOTE: Get the device context
    ComPtr<ID3D11Device> device;
    buffer_->GetDevice(&device);
    ComPtr<ID3D11DeviceContext> deviceContext;
    device->GetImmediateContext(&deviceContext);

    POMDOG_ASSERT(deviceContext != nullptr);

    // NOTE: Map the buffer
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    auto hr = deviceContext->Map(
        buffer_.Get(),
        0,
        mapTypeForWriting_,
        0,
        &mappedResource);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to map buffer");
    }

    auto mappedMemory = reinterpret_cast<u8*>(mappedResource.pData) + offsetInBytes;
    std::memcpy(mappedMemory, source, sizeInBytes);

    deviceContext->Unmap(buffer_.Get(), 0);
}

unsafe_ptr<ID3D11Buffer>
BufferDirect3D11::getBuffer() const noexcept
{
    POMDOG_ASSERT(buffer_);
    return buffer_.Get();
}

std::unique_ptr<Error>
BufferDirect3D11::initialize(
    unsafe_ptr<ID3D11Device> device,
    const BufferDesc& desc,
    std::span<const u8> initialData) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(desc.sizeInBytes > 0);

    if (!initialData.empty()) {
        if (initialData.size() < desc.sizeInBytes) {
            return errors::make("initialData size must be >= desc.sizeInBytes");
        }
    }

    memoryUsage_ = desc.memoryUsage;

    // Determine D3D11 bind flags
    UINT bindFlags = 0;
    if (hasFlag(desc.bindFlags, BufferBindFlags::VertexBuffer)) {
        bindFlags |= D3D11_BIND_VERTEX_BUFFER;
    }
    if (hasFlag(desc.bindFlags, BufferBindFlags::IndexBuffer)) {
        bindFlags |= D3D11_BIND_INDEX_BUFFER;
    }
    if (hasFlag(desc.bindFlags, BufferBindFlags::ConstantBuffer)) {
        bindFlags |= D3D11_BIND_CONSTANT_BUFFER;
    }

    // NOTE: D3D11_MAP_WRITE_NO_OVERWRITE cannot be used on a resource
    // created with D3D11_BIND_CONSTANT_BUFFER. Constant buffers must use
    // D3D11_MAP_WRITE_DISCARD instead.
    // See https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_map
    //
    // Direct3D 11.1 runtime relaxes this restriction via
    // D3D11_FEATURE_DATA_D3D11_OPTIONS::MapNoOverwriteOnDynamicConstantBuffer,
    // but we use DISCARD for maximum compatibility.
    // See https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_feature_data_d3d11_options
    if (bindFlags & D3D11_BIND_CONSTANT_BUFFER) {
        mapTypeForWriting_ = D3D11_MAP_WRITE_DISCARD;
    }
    else {
        mapTypeForWriting_ = D3D11_MAP_WRITE_NO_OVERWRITE;
    }

    // Constant buffer size alignment (must be multiple of 16 bytes)
    auto sizeInBytes = desc.sizeInBytes;
    if ((bindFlags & D3D11_BIND_CONSTANT_BUFFER) && (sizeInBytes % 16 != 0)) {
        const auto offset = sizeInBytes % 16;
        sizeInBytes += (16 - offset);
    }

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = static_cast<::UINT>(sizeInBytes);
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    switch (desc.memoryUsage) {
    case MemoryUsage::GpuOnly:
        if (!initialData.empty()) {
            bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        }
        else {
            bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        }
        bufferDesc.BindFlags = bindFlags;
        bufferDesc.CPUAccessFlags = 0;
        break;
    case MemoryUsage::CpuToGpu:
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.BindFlags = bindFlags;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        break;
    case MemoryUsage::GpuToCpu:
        bufferDesc.Usage = D3D11_USAGE_STAGING;
        bufferDesc.BindFlags = 0; // Staging buffers cannot have bind flags
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        break;
    }

    D3D11_SUBRESOURCE_DATA subresourceData;
    subresourceData.pSysMem = initialData.empty() ? nullptr : initialData.data();
    subresourceData.SysMemPitch = 0;
    subresourceData.SysMemSlicePitch = 0;

    auto initData = (!initialData.empty()) ? &subresourceData : nullptr;

    if (auto hr = device->CreateBuffer(&bufferDesc, initData, &buffer_); FAILED(hr)) {
        return errors::make("failed to create ID3D11Buffer: hr = " + std::to_string(hr));
    }

    return nullptr;
}

std::span<u8> BufferDirect3D11::map(u32 offsetInBytes, u32 sizeInBytes)
{
    POMDOG_ASSERT(buffer_);
    POMDOG_ASSERT(memoryUsage_ != MemoryUsage::GpuOnly);

    ComPtr<ID3D11Device> device;
    buffer_->GetDevice(&device);
    ComPtr<ID3D11DeviceContext> deviceContext;
    device->GetImmediateContext(&deviceContext);
    POMDOG_ASSERT(deviceContext != nullptr);

    D3D11_MAP mapType;
    switch (memoryUsage_) {
    case MemoryUsage::CpuToGpu:
        mapType = mapTypeForWriting_;
        break;
    case MemoryUsage::GpuToCpu:
        mapType = D3D11_MAP_READ;
        break;
    default:
        return {};
    }

    D3D11_MAPPED_SUBRESOURCE mapped;
    auto hr = deviceContext->Map(buffer_.Get(), 0, mapType, 0, &mapped);
    if (FAILED(hr)) {
        return {};
    }

    return std::span<u8>{static_cast<u8*>(mapped.pData) + offsetInBytes, sizeInBytes};
}

void BufferDirect3D11::unmap()
{
    POMDOG_ASSERT(buffer_);

    ComPtr<ID3D11Device> device;
    buffer_->GetDevice(&device);
    ComPtr<ID3D11DeviceContext> deviceContext;
    device->GetImmediateContext(&deviceContext);
    POMDOG_ASSERT(deviceContext != nullptr);

    deviceContext->Unmap(buffer_.Get(), 0);
}

} // namespace pomdog::gpu::detail::direct3d11
