// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativeBuffer.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <wrl/client.h>
#include <d3d12.h>

namespace Pomdog {
namespace Detail {
namespace Direct3D12 {

class BufferDirect3D12 final : public NativeBuffer {
public:
    BufferDirect3D12(ID3D12Device* device,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage);

    BufferDirect3D12(ID3D12Device* device,
        void const* sourceData,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage);

    void GetData(
        std::size_t offsetInBytes,
        void* destination,
        std::size_t sizeInBytes) const override;

    void SetData(
        std::size_t offsetInBytes,
        void const* source,
        std::size_t sizeInBytes) override;

    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
};

} // namespace Direct3D12
} // namespace Detail
} // namespace Pomdog
