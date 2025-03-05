// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/direct3d11/prerequisites_direct3d11.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::direct3d11 {

class Texture2DDirect3D11 final : public Texture2D {
private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView_;
    std::int32_t pixelWidth_ = 0;
    std::int32_t pixelHeight_ = 0;
    std::int32_t levelCount_ = 0;
    PixelFormat format_ = PixelFormat::A8_UNorm;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        unsafe_ptr<ID3D11Device> nativeDevice,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        PixelFormat format) noexcept;

    /// Gets the width of the texture data, in pixels.
    [[nodiscard]] std::int32_t
    getWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    [[nodiscard]] std::int32_t
    getHeight() const noexcept override;

    /// Gets the mipmap level.
    [[nodiscard]] std::int32_t
    getLevelCount() const noexcept override;

    /// Gets the format of the pixel data in the texture.
    [[nodiscard]] PixelFormat
    getFormat() const noexcept override;

    /// Sets texture data.
    void setData(const void* pixelData) override;

    /// Gets the pointer of the shader-resource-view.
    [[nodiscard]] unsafe_ptr<ID3D11ShaderResourceView>
    getShaderResourceView() const noexcept;
};

} // namespace pomdog::gpu::detail::direct3d11
