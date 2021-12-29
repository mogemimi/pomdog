// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/direct3d11/prerequisites_direct3d11.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::direct3d11 {

class Texture2DDirect3D11 final : public Texture2D {
public:
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        ID3D11Device* nativeDevice,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format) noexcept;

    /// Gets the width of the texture data, in pixels.
    std::int32_t GetWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    std::int32_t GetHeight() const noexcept override;

    /// Gets the mipmap level.
    std::int32_t GetLevelCount() const noexcept override;

    /// Gets the format of the pixel data in the texture.
    SurfaceFormat GetFormat() const noexcept override;

    /// Sets texture data.
    void SetData(const void* pixelData) override;

    /// Gets the pointer of the shader-resource-view.
    [[nodiscard]] ID3D11ShaderResourceView*
    GetShaderResourceView() const noexcept;

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
    std::int32_t pixelWidth = 0;
    std::int32_t pixelHeight = 0;
    std::int32_t levelCount = 0;
    SurfaceFormat format = SurfaceFormat::A8_UNorm;
};

} // namespace pomdog::gpu::detail::direct3d11
