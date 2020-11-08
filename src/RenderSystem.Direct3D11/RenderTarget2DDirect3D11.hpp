// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeRenderTarget2D.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include <wrl/client.h>

namespace Pomdog::Detail::Direct3D11 {

class RenderTarget2DDirect3D11 final : public NativeRenderTarget2D {
public:
    RenderTarget2DDirect3D11(
        ID3D11Device* device,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format,
        DepthFormat depthStencilFormat,
        std::int32_t multiSampleCount);

    RenderTarget2DDirect3D11(
        ID3D11Device* device,
        IDXGISwapChain* swapChain,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        DepthFormat depthStencilFormat,
        std::int32_t multiSampleCount);

    void GetData(
        void* result,
        std::size_t offsetInBytes,
        std::size_t sizeInBytes,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format) const override;

    ID3D11RenderTargetView* GetRenderTargetView() const;

    ID3D11DepthStencilView* GetDepthStencilView() const;

    ID3D11ShaderResourceView* GetShaderResourceView() const;

    void ResetBackBuffer(
        ID3D11Device* device,
        IDXGISwapChain* swapChain,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        DepthFormat depthStencilFormat);

    void ResetBackBuffer();

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureResourceView;
};

} // namespace Pomdog::Detail::Direct3D11
