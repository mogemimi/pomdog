// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_VERTEXBUFFERDIRECT3D11_EABF11E7_HPP
#define POMDOG_VERTEXBUFFERDIRECT3D11_EABF11E7_HPP

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeVertexBuffer.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {

class VertexBufferDirect3D11 final: public NativeVertexBuffer {
public:
    VertexBufferDirect3D11() = delete;

    VertexBufferDirect3D11(ID3D11Device* nativeDevice,
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> const& deviceContext,
        std::uint32_t sizeInBytes, BufferUsage bufferUsage);

    VertexBufferDirect3D11(ID3D11Device* nativeDevice,
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> const& deviceContext,
        void const* vertices, std::uint32_t sizeInBytes, BufferUsage bufferUsage);

    ///@copydoc NativeVertexBuffer
    void SetData(std::uint32_t offsetInBytes, void const* source,
        std::uint32_t sizeInBytes) override;

    ID3D11Buffer* NativeBuffer() const;

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> nativeVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_VERTEXBUFFERDIRECT3D11_EABF11E7_HPP
