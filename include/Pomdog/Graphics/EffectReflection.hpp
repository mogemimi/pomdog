// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTREFLECTION_88543BAB_HPP
#define POMDOG_EFFECTREFLECTION_88543BAB_HPP

#include "detail/ForwardDeclarations.hpp"
#include "EffectConstantDescription.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <vector>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeEffectReflection;

}// namespace RenderSystem
}// namespace Detail

class POMDOG_EXPORT EffectReflection {
public:
    EffectReflection() = delete;
    EffectReflection(EffectReflection const&) = delete;
    EffectReflection(EffectReflection &&) = default;

    EffectReflection(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        std::shared_ptr<EffectPass> const& effectPass);

    ~EffectReflection();

    std::vector<EffectConstantDescription> GetConstantBuffers() const;

public:
    Detail::RenderSystem::NativeEffectReflection* NativeEffectReflection();

private:
    std::unique_ptr<Detail::RenderSystem::NativeEffectReflection> nativeEffectReflection;
};

}// namespace Pomdog

#endif // POMDOG_EFFECTREFLECTION_88543BAB_HPP
