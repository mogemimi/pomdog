// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_HARDWAREBUFFERHELPER_3C0164CA_35BF_4EFF_B716_BC9058257CFA_HPP
#define POMDOG_HARDWAREBUFFERHELPER_3C0164CA_35BF_4EFF_B716_BC9058257CFA_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "PrerequisitesDirect3D11.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstddef>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {

class HardwareBufferHelper {
public:
    static Microsoft::WRL::ComPtr<ID3D11Buffer> CreateVertexBuffer(ID3D11Device* nativeDevice,
        void const* data, std::size_t sizeInBytes, BufferUsage bufferUsage);

    static Microsoft::WRL::ComPtr<ID3D11Buffer> CreateIndexBuffer(ID3D11Device* nativeDevice,
        void const* data, std::size_t sizeInBytes, BufferUsage bufferUsage);

    static Microsoft::WRL::ComPtr<ID3D11Buffer> CreateConstantBuffer(ID3D11Device* nativeDevice,
        void const* data, std::size_t sizeInBytes, BufferUsage bufferUsage);

    static void SetData(ID3D11Buffer* buffer, ID3D11DeviceContext* deviceContext,
        std::size_t offsetInBytes, void const* source, std::size_t sizeInBytes);

    static void GetData(ID3D11Buffer* buffer, ID3D11DeviceContext* deviceContext,
        std::size_t offsetInBytes, std::size_t sizeInBytes, void* output);
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_HARDWAREBUFFERHELPER_3C0164CA_35BF_4EFF_B716_BC9058257CFA_HPP)
