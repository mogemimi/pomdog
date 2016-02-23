// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativeRenderTarget2D.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <wrl/client.h>
#include <d3d12.h>

namespace Pomdog {
namespace Detail {
namespace Direct3D12 {

class RenderTarget2DDirect3D12 final : public NativeRenderTarget2D {
public:
    RenderTarget2DDirect3D12(
        ID3D12Device* device,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format,
        DepthFormat depthStencilFormat,
        std::int32_t multiSampleCount);

    RenderTarget2DDirect3D12(
        ID3D12Device* device,
        IDXGISwapChain* swapChain,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        DXGI_FORMAT dxgiFormat,
        DepthFormat depthStencilFormat,
        std::int32_t multiSampleCount);

    ID3D12DescriptorHeap* GetRenderTargetDescriptorHeap() const;

    ID3D12DescriptorHeap* GetDepthStencilDescriptorHeap() const;

    ID3D12DescriptorHeap* GetShaderResourceViewHeap() const;

    ID3D12Resource* GetTexture2D() const;

    void ResetBackBuffer(
        ID3D12Device* device,
        IDXGISwapChain* swapChain,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        DepthFormat depthStencilFormat);

    void ResetBackBuffer();

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> texture2D;
    Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> renderTargetViewHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> depthStencilViewHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> shaderResourceViewHeap;
};

} // namespace Direct3D12
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_RENDERTARGET2DDIRECT3D12_536D88A0_HPP
