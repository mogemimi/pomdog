// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeTexture2D.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <wrl/client.h>

namespace Pomdog {
namespace Detail {
namespace Direct3D11 {

class Texture2DDirect3D11 final : public NativeTexture2D {
public:
    Texture2DDirect3D11(
        ID3D11Device* nativeDevice,
        const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format);

    void SetData(
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format,
        const void* pixelData) override;

    ID3D11ShaderResourceView* GetShaderResourceView() const;

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
};

} // namespace Direct3D11
} // namespace Detail
} // namespace Pomdog
