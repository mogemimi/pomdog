// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/shader_reflections/effect_reflection.h"
#import <Metal/Metal.h>

namespace pomdog::gpu::detail::metal {

class EffectReflectionMetal final : public EffectReflection {
public:
    explicit EffectReflectionMetal(MTLRenderPipelineReflection* reflectionIn);

    [[nodiscard]] std::vector<EffectConstantDescription>
    getConstantBuffers() const noexcept override;

private:
    __strong MTLRenderPipelineReflection* reflection_ = nullptr;
};

} // namespace pomdog::gpu::detail::metal
