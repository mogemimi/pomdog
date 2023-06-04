// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/direct3d11/prerequisites_direct3d11.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::direct3d11 {

class PipelineStateDirect3D11 final : public PipelineState {
public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        ID3D11Device* device,
        const PipelineDescriptor& descriptor) noexcept;

    void apply(ID3D11DeviceContext* deviceContext, FLOAT const blendFactor[4]);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;
    Microsoft::WRL::ComPtr<ID3D11BlendState> blendState_;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState_;
    UINT sampleMask_ = 0;
    D3D11_PRIMITIVE_TOPOLOGY primitiveTopology_ = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
};

} // namespace pomdog::gpu::detail::direct3d11
