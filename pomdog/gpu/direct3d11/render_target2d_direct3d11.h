// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/direct3d11/prerequisites_direct3d11.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/render_target2d.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::direct3d11 {

class RenderTarget2DDirect3D11 final : public RenderTarget2D {
public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        ID3D11Device* device,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        PixelFormat format,
        std::int32_t multiSampleCount) noexcept;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        ID3D11Device* device,
        IDXGISwapChain* swapChain,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        PixelFormat format,
        std::int32_t multiSampleCount) noexcept;

    /// Gets the width of the texture data, in pixels.
    [[nodiscard]] std::int32_t
    getWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    [[nodiscard]] std::int32_t
    getHeight() const noexcept override;

    /// Gets the mipmap level.
    [[nodiscard]] std::int32_t
    getLevelCount() const noexcept override;

    /// Gets the format of the pixel data in the render target.
    [[nodiscard]] PixelFormat
    getFormat() const noexcept override;

    /// Gets the size of the texture resource.
    [[nodiscard]] Rectangle
    getBounds() const noexcept override;

    /// Copies the pixel data from texture to memory.
    void getData(void* result, std::size_t offsetInBytes, std::size_t sizeInBytes) const override;

    /// Gets the pointer of the render-target-view.
    [[nodiscard]] ID3D11RenderTargetView*
    getRenderTargetView() const noexcept;

    /// Gets the pointer of the shader-resource-view.
    [[nodiscard]] ID3D11ShaderResourceView*
    getShaderResourceView() const noexcept;

    [[nodiscard]] std::unique_ptr<Error>
    resetBackBuffer(
        ID3D11Device* device,
        IDXGISwapChain* swapChain,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight) noexcept;

    void resetBackBuffer() noexcept;

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D_;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureResourceView_;
    std::int32_t pixelWidth_ = 0;
    std::int32_t pixelHeight_ = 0;
    std::int32_t levelCount_ = 0;
    PixelFormat format_ = PixelFormat::A8_UNorm;
    bool generateMipmap_ = false;
    bool multiSampleEnabled_ = false;
};

} // namespace pomdog::gpu::detail::direct3d11
