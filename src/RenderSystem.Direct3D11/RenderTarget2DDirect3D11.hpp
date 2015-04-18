// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_RENDERTARGET2DDIRECT3D11_75264088_HPP
#define POMDOG_RENDERTARGET2DDIRECT3D11_75264088_HPP

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeRenderTarget2D.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <wrl/client.h>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {

class RenderTarget2DDirect3D11 final: public NativeRenderTarget2D {
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

    ID3D11RenderTargetView* RenderTargetView() const;

    ID3D11DepthStencilView* DepthStencilView() const;

    ID3D11ShaderResourceView* ShaderResourceView() const;

    void ResetBackBuffer(
        ID3D11Device* device,
        IDXGISwapChain* swapChain,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        DepthFormat depthStencilFormat);

    void ResetBackBuffer();

private:
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTexture;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureResourceView;
};

} // namespace Direct3D11
} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_RENDERTARGET2DDIRECT3D11_75264088_HPP
