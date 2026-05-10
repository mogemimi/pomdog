// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/direct3d11/prerequisites_direct3d11.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::direct3d11 {

/// Texture2DDirect3D11 is the Direct3D 11 implementation of Texture2D.
class Texture2DDirect3D11 final : public Texture2D {
private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView_;
    i32 pixelWidth_ = 0;
    i32 pixelHeight_ = 0;
    i32 levelCount_ = 0;
    PixelFormat format_ = PixelFormat::A8_UNorm;

public:
    /// Creates a D3D11 texture resource and its shader-resource-view.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        unsafe_ptr<ID3D11Device> nativeDevice,
        i32 pixelWidth,
        i32 pixelHeight,
        i32 levelCount,
        PixelFormat format) noexcept;

    /// Gets the width of the texture data, in pixels.
    [[nodiscard]] i32
    getWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    [[nodiscard]] i32
    getHeight() const noexcept override;

    /// Gets the mipmap level.
    [[nodiscard]] i32
    getLevelCount() const noexcept override;

    /// Gets the format of the pixel data in the texture.
    [[nodiscard]] PixelFormat
    getFormat() const noexcept override;

    /// Sets texture data.
    void setData(const void* pixelData) override;

    /// Sets texture data for a sub-region.
    void setData(
        i32 mipLevel,
        const Rect2D& region,
        const void* pixelData,
        u32 bytesPerRow) override;

    /// Gets the pointer of the shader-resource-view.
    [[nodiscard]] unsafe_ptr<ID3D11ShaderResourceView>
    getShaderResourceView() const noexcept;
};

} // namespace pomdog::gpu::detail::direct3d11
