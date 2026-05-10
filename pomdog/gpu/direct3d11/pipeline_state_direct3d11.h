// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/direct3d11/prerequisites_direct3d11.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
struct PipelineDesc;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::direct3d11 {

/// PipelineStateDirect3D11 is the Direct3D 11 implementation of PipelineState.
class PipelineStateDirect3D11 final : public PipelineState {
public:
    /// Compiles and creates all D3D11 state objects from the pipeline descriptor.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        unsafe_ptr<ID3D11Device> device,
        const PipelineDesc& descriptor) noexcept;

    /// Binds all compiled D3D11 state objects to the given device context.
    void apply(
        unsafe_ptr<ID3D11DeviceContext> deviceContext,
        FLOAT const blendFactor[4]);

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
