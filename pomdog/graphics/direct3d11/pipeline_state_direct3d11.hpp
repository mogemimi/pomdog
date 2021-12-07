// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/graphics/direct3d11/prerequisites_direct3d11.hpp"
#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/pipeline_state.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::Direct3D11 {

class PipelineStateDirect3D11 final : public PipelineState {
public:
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        ID3D11Device* device,
        const PipelineStateDescription& description) noexcept;

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
