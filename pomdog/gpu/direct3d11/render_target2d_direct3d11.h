// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/direct3d11/prerequisites_direct3d11.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/render_target2d.h"
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::direct3d11 {

class RenderTarget2DDirect3D11 final : public RenderTarget2D {
public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        unsafe_ptr<ID3D11Device> device,
        i32 pixelWidth,
        i32 pixelHeight,
        i32 levelCount,
        PixelFormat format,
        i32 multiSampleCount) noexcept;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        unsafe_ptr<ID3D11Device> device,
        unsafe_ptr<IDXGISwapChain> swapChain,
        i32 pixelWidth,
        i32 pixelHeight,
        i32 levelCount,
        PixelFormat format,
        i32 multiSampleCount) noexcept;

    /// Gets the width of the texture data, in pixels.
    [[nodiscard]] i32
    getWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    [[nodiscard]] i32
    getHeight() const noexcept override;

    /// Gets the mipmap level.
    [[nodiscard]] i32
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
    [[nodiscard]] unsafe_ptr<ID3D11RenderTargetView>
    getRenderTargetView() const noexcept;

    /// Gets the pointer of the shader-resource-view.
    [[nodiscard]] unsafe_ptr<ID3D11ShaderResourceView>
    getShaderResourceView() const noexcept;

    [[nodiscard]] std::unique_ptr<Error>
    resetBackBuffer(
        unsafe_ptr<ID3D11Device> device,
        unsafe_ptr<IDXGISwapChain> swapChain,
        i32 pixelWidth,
        i32 pixelHeight) noexcept;

    void resetBackBuffer() noexcept;

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D_;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureResourceView_;
    i32 pixelWidth_ = 0;
    i32 pixelHeight_ = 0;
    i32 levelCount_ = 0;
    PixelFormat format_ = PixelFormat::A8_UNorm;
    bool generateMipmap_ = false;
    bool multiSampleEnabled_ = false;
};

} // namespace pomdog::gpu::detail::direct3d11
