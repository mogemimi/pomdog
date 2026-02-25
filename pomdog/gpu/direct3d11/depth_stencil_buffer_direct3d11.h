// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/depth_stencil_buffer.h"
#include "pomdog/gpu/direct3d11/prerequisites_direct3d11.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::direct3d11 {

class DepthStencilBufferDirect3D11 final : public DepthStencilBuffer {
private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil_;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView_;
    i32 pixelWidth_ = 0;
    i32 pixelHeight_ = 0;
    PixelFormat depthStencilFormat_ = PixelFormat::Invalid;
    bool multiSampleEnabled_ = false;

public:
    /// Initializes the depth stencil buffer.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        unsafe_ptr<ID3D11Device> device,
        i32 pixelWidth,
        i32 pixelHeight,
        PixelFormat depthStencilFormat,
        i32 multiSampleCount) noexcept;

    /// Gets the width of the texture data, in pixels.
    [[nodiscard]] i32
    getWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    [[nodiscard]] i32
    getHeight() const noexcept override;

    /// Gets the format of the pixel data in the depth-stencil buffer.
    [[nodiscard]] PixelFormat
    getFormat() const noexcept override;

    /// Gets the size of the texture resource.
    [[nodiscard]] Rect2D
    getBounds() const noexcept override;

    /// Gets the pointer of the depth-stencil-view.
    [[nodiscard]] unsafe_ptr<ID3D11DepthStencilView>
    getDepthStencilView() const noexcept;

    /// Resets the depth stencil buffer. This method is used when the swap chain is resized.
    [[nodiscard]] std::unique_ptr<Error>
    resetBuffer(
        unsafe_ptr<ID3D11Device> device,
        i32 pixelWidth,
        i32 pixelHeight,
        PixelFormat depthStencilFormat,
        i32 multiSampleCount) noexcept;
};

} // namespace pomdog::gpu::detail::direct3d11
