// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/graphics/depth_stencil_buffer.hpp"
#include "pomdog/graphics/direct3d11/prerequisites_direct3d11.hpp"
#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/surface_format.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::Direct3D11 {

class DepthStencilBufferDirect3D11 final : public DepthStencilBuffer {
public:
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        ID3D11Device* device,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        SurfaceFormat depthStencilFormat,
        std::int32_t multiSampleCount) noexcept;

    /// Gets the width of the texture data, in pixels.
    std::int32_t GetWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    std::int32_t GetHeight() const noexcept override;

    /// Gets the format of the pixel data in the depth-stencil buffer.
    SurfaceFormat GetFormat() const noexcept override;

    /// Gets the size of the texture resource.
    Rectangle GetBounds() const noexcept override;

    /// Gets the pointer of the depth-stencil-view.
    [[nodiscard]] ID3D11DepthStencilView*
    GetDepthStencilView() const noexcept;

    [[nodiscard]] std::unique_ptr<Error>
    ResetBuffer(
        ID3D11Device* device,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        SurfaceFormat depthStencilFormat,
        std::int32_t multiSampleCount) noexcept;

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    std::int32_t pixelWidth = 0;
    std::int32_t pixelHeight = 0;
    SurfaceFormat depthStencilFormat = SurfaceFormat::Invalid;
    bool multiSampleEnabled = false;
};

} // namespace Pomdog::Detail::Direct3D11
