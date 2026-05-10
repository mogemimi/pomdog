// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Metal/MTLDepthStencil.h>
#import <Metal/MTLRenderCommandEncoder.h>
#import <Metal/MTLRenderPipeline.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
struct PipelineDesc;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::metal {

/// RasterizerStateMetal holds the Metal rasterizer state applied per draw call.
struct RasterizerStateMetal final {
    f32 depthBias = 0;
    f32 slopeScaledDepthBias = 0;
    MTLCullMode cullMode = MTLCullModeFront;
    MTLTriangleFillMode fillMode = MTLTriangleFillModeFill;
};

/// PipelineStateMetal is the Metal implementation of PipelineState.
class PipelineStateMetal final : public PipelineState {
private:
    id<MTLRenderPipelineState> pipelineState_ = nullptr;
    id<MTLDepthStencilState> depthStencilState_ = nullptr;
    __strong MTLRenderPipelineReflection* reflection_ = nullptr;
    RasterizerStateMetal rasterizerState_ = {};
    MTLPrimitiveType primitiveType_ = MTLPrimitiveTypeTriangle;

public:
    ~PipelineStateMetal() override;

    /// Compiles and creates all Metal state objects from the pipeline descriptor.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        id<MTLDevice> device,
        const PipelineDesc& descriptor) noexcept;

    /// Binds the pipeline state, depth/stencil state, and rasterizer state to the encoder.
    void apply(id<MTLRenderCommandEncoder> commandEncoder);

    /// Returns the primitive topology as an MTLPrimitiveType.
    [[nodiscard]] MTLPrimitiveType
    getPrimitiveType() const noexcept;
};

} // namespace pomdog::gpu::detail::metal
