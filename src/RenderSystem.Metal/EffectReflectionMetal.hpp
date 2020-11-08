// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativeEffectReflection.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#import <Metal/Metal.h>

namespace Pomdog::Detail::Metal {

class EffectReflectionMetal final : public NativeEffectReflection {
public:
    explicit EffectReflectionMetal(MTLRenderPipelineReflection* reflectionIn);

    std::vector<EffectConstantDescription> GetConstantBuffers() const override;

private:
    __strong MTLRenderPipelineReflection* reflection;
};

} // namespace Pomdog::Detail::Metal
