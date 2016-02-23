// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativeTexture2D.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <wrl/client.h>
#include <d3d12.h>

namespace Pomdog {
namespace Detail {
namespace Direct3D12 {

class Texture2DDirect3D12 final : public NativeTexture2D {
public:
    Texture2DDirect3D12(
        ID3D12Device* device,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format);

    void SetData(
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format,
        void const* pixelData) override;

    ID3D12DescriptorHeap* GetShaderResourceViewHeap() const;

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> texture2D;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> shaderResourceViewHeap;
};

} // namespace Direct3D12
} // namespace Detail
} // namespace Pomdog
