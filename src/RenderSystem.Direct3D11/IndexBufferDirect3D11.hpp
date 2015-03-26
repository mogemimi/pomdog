// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_INDEXBUFFERDIRECT3D11_94E97F4A_C7E4_4128_9B93_BB47AFF63697_HPP
#define POMDOG_INDEXBUFFERDIRECT3D11_94E97F4A_C7E4_4128_9B93_BB47AFF63697_HPP

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeIndexBuffer.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {

class IndexBufferDirect3D11 final: public NativeIndexBuffer {
public:
    IndexBufferDirect3D11() = delete;

    IndexBufferDirect3D11(ID3D11Device* nativeDevice,
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> const& deviceContext,
        std::uint32_t sizeInBytes, BufferUsage bufferUsage);

    IndexBufferDirect3D11(ID3D11Device* nativeDevice,
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> const& deviceContext,
        void const* indices, std::uint32_t sizeInBytes, BufferUsage bufferUsage);

    void SetData(std::uint32_t offsetInBytes,
        void const* source, std::uint32_t sizeInBytes) override;

    ID3D11Buffer* NativeBuffer() const;

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> nativeIndexBuffer;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_INDEXBUFFERDIRECT3D11_94E97F4A_C7E4_4128_9B93_BB47AFF63697_HPP)
