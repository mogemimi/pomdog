// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativePipelineState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <wrl/client.h>

namespace Pomdog::Detail::Direct3D11 {

class PipelineStateDirect3D11 final : public NativePipelineState {
public:
    PipelineStateDirect3D11(
        ID3D11Device* device,
        const PipelineStateDescription& description);

    void Apply(ID3D11DeviceContext* deviceContext, FLOAT const blendFactor[4]);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
    Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
    UINT sampleMask;
    D3D11_PRIMITIVE_TOPOLOGY primitiveTopology;
};

} // namespace Pomdog::Detail::Direct3D11
