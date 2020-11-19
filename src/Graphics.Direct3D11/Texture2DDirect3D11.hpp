// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D11.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include <wrl/client.h>

namespace Pomdog::Detail::Direct3D11 {

class Texture2DDirect3D11 final : public Texture2D {
public:
    Texture2DDirect3D11(
        ID3D11Device* nativeDevice,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format);

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

} // namespace Pomdog::Detail::Direct3D11
