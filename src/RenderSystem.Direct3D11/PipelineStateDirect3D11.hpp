// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_PIPELINESTATEDIRECT3D11_0A0A09ED_HPP
#define POMDOG_PIPELINESTATEDIRECT3D11_0A0A09ED_HPP

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativePipelineState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <wrl/client.h>

namespace Pomdog {
namespace Detail {
namespace Direct3D11 {

class PipelineStateDirect3D11 final : public NativePipelineState {
public:
    PipelineStateDirect3D11(ID3D11Device* device,
        PipelineStateDescription const& description);

    void Apply(ID3D11DeviceContext * deviceContext, FLOAT const blendFactor[4]);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
    Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
    UINT sampleMask;
};

} // namespace Direct3D11
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_PIPELINESTATEDIRECT3D11_0A0A09ED_HPP
