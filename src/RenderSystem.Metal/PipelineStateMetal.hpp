// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativePipelineState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#import <Metal/MTLDepthStencil.h>
#import <Metal/MTLRenderCommandEncoder.h>
#import <Metal/MTLRenderPipeline.h>

namespace Pomdog {
namespace Detail {
namespace Metal {

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

private:
    id<MTLRenderPipelineState> pipelineState;
    id<MTLDepthStencilState> depthStencilState;
    __strong MTLRenderPipelineReflection* reflection;
    RasterizerStateMetal rasterizerState;
};

} // namespace Metal
} // namespace Detail
} // namespace Pomdog
