// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Utility/Errors.hpp"
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
    [[nodiscard]] std::shared_ptr<Error>
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
