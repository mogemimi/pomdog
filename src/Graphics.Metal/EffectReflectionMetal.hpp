// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/EffectReflection.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#import <Metal/Metal.h>

namespace Pomdog::Detail::Metal {

class EffectReflectionMetal final : public EffectReflection {
public:
    explicit EffectReflectionMetal(MTLRenderPipelineReflection* reflectionIn);

    std::vector<EffectConstantDescription>
    GetConstantBuffers() const noexcept override;

private:
    __strong MTLRenderPipelineReflection* reflection = nullptr;
};

} // namespace Pomdog::Detail::Metal
