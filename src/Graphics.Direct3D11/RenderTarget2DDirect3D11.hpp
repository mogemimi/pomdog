// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D11.hpp"
#include "DepthStencilBufferDirect3D11.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <wrl/client.h>

namespace Pomdog::Detail::Direct3D11 {

class RenderTarget2DDirect3D11 final : public RenderTarget2D {
public:
    [[nodiscard]] std::shared_ptr<Error>
    Initialize(
        ID3D11Device* device,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format,
        SurfaceFormat depthStencilFormat,
        std::int32_t multiSampleCount) noexcept;

    [[nodiscard]] std::shared_ptr<Error>
    Initialize(
        ID3D11Device* device,
        IDXGISwapChain* swapChain,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format,
        SurfaceFormat depthStencilFormat,
        std::int32_t multiSampleCount) noexcept;

    /// Gets the width of the texture data, in pixels.
    std::int32_t GetWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    std::int32_t GetHeight() const noexcept override;

    /// Gets the mipmap level.
    std::int32_t GetLevelCount() const noexcept override;

    /// Gets the format of the pixel data in the render target.
    SurfaceFormat GetFormat() const noexcept override;

    /// Gets the format of the pixel data in the depth-stencil buffer.
    SurfaceFormat GetDepthStencilFormat() const noexcept override;

    /// Gets the size of the texture resource.
    Rectangle GetBounds() const noexcept override;

    /// Copies the pixel data from texture to memory.
    void GetData(void* result, std::size_t offsetInBytes, std::size_t sizeInBytes) const override;

    /// Gets the pointer of the render-target-view.
    [[nodiscard]] ID3D11RenderTargetView*
    GetRenderTargetView() const noexcept;

    /// Gets the pointer of the depth-stencil-view.
    [[nodiscard]] ID3D11DepthStencilView*
    GetDepthStencilView() const noexcept;

    /// Gets the pointer of the shader-resource-view.
    [[nodiscard]] ID3D11ShaderResourceView*
    GetShaderResourceView() const noexcept;

    [[nodiscard]] std::shared_ptr<Error>
    ResetBackBuffer(
        ID3D11Device* device,
        IDXGISwapChain* swapChain,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        SurfaceFormat depthStencilFormat) noexcept;

    void ResetBackBuffer() noexcept;

private:
    DepthStencilBufferDirect3D11 depthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureResourceView;
    std::int32_t pixelWidth = 0;
    std::int32_t pixelHeight = 0;
    std::int32_t levelCount = 0;
    SurfaceFormat format = SurfaceFormat::A8_UNorm;
    bool generateMipmap = false;
    bool multiSampleEnabled = false;
};

} // namespace Pomdog::Detail::Direct3D11
