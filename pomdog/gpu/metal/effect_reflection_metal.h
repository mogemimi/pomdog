// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/effect_reflection.h"
#include "pomdog/gpu/forward_declarations.h"
#import <Metal/Metal.h>

namespace pomdog::gpu::detail::metal {

class EffectReflectionMetal final : public EffectReflection {
public:
    explicit EffectReflectionMetal(MTLRenderPipelineReflection* reflectionIn);

    std::vector<EffectConstantDescription>
    GetConstantBuffers() const noexcept override;

private:
    __strong MTLRenderPipelineReflection* reflection = nullptr;
};

} // namespace pomdog::gpu::detail::metal
