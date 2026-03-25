// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/direct3d11/buffer_direct3d11.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/buffer_bind_flags.h"
#include "pomdog/gpu/buffer_desc.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/memory_usage.h"
#include "pomdog/logging/log.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/exception.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using Microsoft::WRL::ComPtr;

namespace pomdog::gpu::detail::direct3d11 {
namespace {

[[nodiscard]] std::tuple<ComPtr<ID3D11Buffer>, std::unique_ptr<Error>>
createNativeBuffer(
    unsafe_ptr<ID3D11Device> device,
    u32 sizeInBytes,
    const void* data,
    BufferUsage bufferUsage,
    D3D11_BIND_FLAG bindFlag) noexcept
{
    POMDOG_ASSERT(
        bindFlag == D3D11_BIND_CONSTANT_BUFFER ||
        bindFlag == D3D11_BIND_INDEX_BUFFER ||
        bindFlag == D3D11_BIND_VERTEX_BUFFER);

    if (bindFlag == D3D11_BIND_CONSTANT_BUFFER) {
        if (sizeInBytes > D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT) {
            auto err = errors::make(
                "You must set the sizeInBytes value less than or"
                " equal to D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT.");
            return std::make_tuple(nullptr, std::move(err));
        }

        if (sizeInBytes % 16 != 0) {
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
            Log::Warning("pomdog",
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
    POMDOG_ASSERT(
        (bufferDesc.Usage != D3D11_USAGE_IMMUTABLE) ||
        ((bufferDesc.Usage == D3D11_USAGE_IMMUTABLE) && (data != nullptr)));

    auto initialData = (data != nullptr) ? &subresourceData : nullptr;

    ComPtr<ID3D11Buffer> buffer = nullptr;
    if (auto hr = device->CreateBuffer(&bufferDesc, initialData, &buffer); FAILED(hr)) {
        auto err = errors::make("failed to create ID3D11Buffer");
        return std::make_tuple(nullptr, std::move(err));
    }

    return std::make_tuple(std::move(buffer), nullptr);
}

[[nodiscard]] D3D11_MAP
getMapTypeForWriting(D3D11_BIND_FLAG bindFlag) noexcept
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

} // namespace

std::unique_ptr<Error>
BufferDirect3D11::initialize(
    unsafe_ptr<ID3D11Device> device,
    u32 sizeInBytes,
    BufferUsage bufferUsage,
    D3D11_BIND_FLAG bindFlag) noexcept
{
    memoryUsage_ = (bufferUsage == BufferUsage::Immutable)
                       ? MemoryUsage::GpuOnly
                       : MemoryUsage::CpuToGpu;
    mapTypeForWriting_ = getMapTypeForWriting(bindFlag);

    std::unique_ptr<Error> err;
    std::tie(buffer_, err) = createNativeBuffer(
        device,
        sizeInBytes,
        nullptr,
        bufferUsage,
        bindFlag);
    if (err != nullptr) {
        return errors::wrap(std::move(err), "CreateNativeBuffer() failed");
    }

    return nullptr;
}

std::unique_ptr<Error>
BufferDirect3D11::initialize(
    unsafe_ptr<ID3D11Device> device,
    const void* sourceData,
    u32 sizeInBytes,
    BufferUsage bufferUsage,
    D3D11_BIND_FLAG bindFlag) noexcept
{
    memoryUsage_ = (bufferUsage == BufferUsage::Immutable)
                       ? MemoryUsage::GpuOnly
                       : MemoryUsage::CpuToGpu;
    mapTypeForWriting_ = getMapTypeForWriting(bindFlag);

    std::unique_ptr<Error> err;
    std::tie(buffer_, err) = createNativeBuffer(
        device,
        sizeInBytes,
        sourceData,
        bufferUsage,
        bindFlag);
    if (err != nullptr) {
        return errors::wrap(std::move(err), "CreateNativeBuffer() failed");
    }

    return nullptr;
}

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

    // Determine map type for writing
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
