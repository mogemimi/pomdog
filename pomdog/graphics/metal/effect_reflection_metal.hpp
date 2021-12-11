// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/effect_reflection.hpp"
#include "pomdog/graphics/forward_declarations.hpp"
#import <Metal/Metal.h>

namespace pomdog::detail::metal {

class EffectReflectionMetal final : public EffectReflection {
public:
    explicit EffectReflectionMetal(MTLRenderPipelineReflection* reflectionIn);

    std::vector<EffectConstantDescription>
    GetConstantBuffers() const noexcept override;

private:
    __strong MTLRenderPipelineReflection* reflection = nullptr;
};

} // namespace pomdog::detail::metal