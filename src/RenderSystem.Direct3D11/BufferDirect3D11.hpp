// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeBuffer.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <wrl/client.h>

namespace Pomdog {
namespace Detail {
namespace Direct3D11 {

class BufferDirect3D11 final : public NativeBuffer {
public:
    BufferDirect3D11(
        ID3D11Device* device,
        const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        D3D11_BIND_FLAG bindFlag);

    BufferDirect3D11(
        ID3D11Device* devuce,
        const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext,
        const void* sourceData,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        D3D11_BIND_FLAG bindFlag);

    void GetData(
        std::size_t offsetInBytes,
        void* destination,
        std::size_t sizeInBytes) const override;

    void SetData(
        std::size_t offsetInBytes,
        const void* sourceData,
        std::size_t sizeInBytes) override;

    ID3D11Buffer* GetBuffer() const;

private:
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    D3D11_MAP mapTypeForWriting;
};

} // namespace Direct3D11
} // namespace Detail
} // namespace Pomdog
