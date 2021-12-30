// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/utility/errors.h"
#import <Metal/MTLDepthStencil.h>
#import <Metal/MTLRenderCommandEncoder.h>
#import <Metal/MTLRenderPipeline.h>

namespace pomdog::gpu::detail::metal {

struct RasterizerStateMetal final {
    float depthBias = 0;
    float slopeScaledDepthBias = 0;
    MTLCullMode cullMode = MTLCullModeFront;
    MTLTriangleFillMode fillMode = MTLTriangleFillModeFill;
};

class PipelineStateMetal final : public PipelineState {
public:
    ~PipelineStateMetal() override;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        id<MTLDevice> device,
        const PipelineDescriptor& descriptor) noexcept;

    void Apply(id<MTLRenderCommandEncoder> commandEncoder);

    MTLPrimitiveType GetPrimitiveType() const noexcept;

private:
    id<MTLRenderPipelineState> pipelineState = nullptr;
    id<MTLDepthStencilState> depthStencilState = nullptr;
    __strong MTLRenderPipelineReflection* reflection = nullptr;
    RasterizerStateMetal rasterizerState;
    MTLPrimitiveType primitiveType;
};

} // namespace pomdog::gpu::detail::metal
