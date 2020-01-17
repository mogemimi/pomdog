// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativePipelineState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
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

class PipelineStateMetal final : public NativePipelineState {
public:
    PipelineStateMetal(
        id<MTLDevice> device,
        const PipelineStateDescription& description);

    void Apply(id<MTLRenderCommandEncoder> commandEncoder);

    MTLPrimitiveType GetPrimitiveType() const noexcept;

private:
    id<MTLRenderPipelineState> pipelineState;
    id<MTLDepthStencilState> depthStencilState;
    __strong MTLRenderPipelineReflection* reflection;
    RasterizerStateMetal rasterizerState;
    MTLPrimitiveType primitiveType;
};

} // namespace Pomdog::Detail::Metal
