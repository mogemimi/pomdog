// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTPASS_3FA791E9_HPP
#define POMDOG_EFFECTPASS_3FA791E9_HPP

#include "detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT EffectPass {
public:
    EffectPass() = delete;
    EffectPass(EffectPass const&) = delete;
    EffectPass(EffectPass &&) = default;

    EffectPass(GraphicsDevice & graphicsDevice,
        PipelineStateDescription const& description);

    EffectPass(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        PipelineStateDescription const& description);

    ~EffectPass();

    EffectPass & operator=(EffectPass const&) = delete;
    EffectPass & operator=(EffectPass &&) = default;

public:
    Detail::RenderSystem::NativePipelineState* NativePipelineState();

private:
    std::unique_ptr<Detail::RenderSystem::NativePipelineState> nativePipelineState;
};

} // namespace Pomdog

#endif // POMDOG_EFFECTPASS_3FA791E9_HPP
