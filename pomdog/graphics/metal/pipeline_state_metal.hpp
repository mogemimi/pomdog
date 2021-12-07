// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/pipeline_state.hpp"
#include "pomdog/utility/errors.hpp"
#import <Metal/MTLDepthStencil.h>
#import <Metal/MTLRenderCommandEncoder.h>
#import <Metal/MTLRenderPipeline.h>

namespace Pomdog::Detail::Metal {

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
        const PipelineStateDescription& description) noexcept;

    void Apply(id<MTLRenderCommandEncoder> commandEncoder);

    MTLPrimitiveType GetPrimitiveType() const noexcept;

private:
    id<MTLRenderPipelineState> pipelineState = nullptr;
    id<MTLDepthStencilState> depthStencilState = nullptr;
    __strong MTLRenderPipelineReflection* reflection = nullptr;
    RasterizerStateMetal rasterizerState;
    MTLPrimitiveType primitiveType;
};

} // namespace Pomdog::Detail::Metal
