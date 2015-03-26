// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTPASS_3FA791E9_F535_41AF_B197_3DF71A12085A_HPP
#define POMDOG_EFFECTPASS_3FA791E9_F535_41AF_B197_3DF71A12085A_HPP

#include "detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <unordered_map>
#include <string>
#include <memory>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeEffectPass;

}// namespace RenderSystem
}// namespace Detail

class POMDOG_EXPORT EffectPass {
public:
    EffectPass() = delete;
    EffectPass(EffectPass const&) = delete;
    EffectPass(EffectPass &&) = default;

    EffectPass(GraphicsDevice & graphicsDevice,
        EffectPassDescription const& description);

    EffectPass(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        EffectPassDescription const& description);

    ~EffectPass();

    EffectPass & operator=(EffectPass const&) = delete;
    EffectPass & operator=(EffectPass &&) = default;

public:
    Detail::RenderSystem::NativeEffectPass* NativeEffectPass();

private:
    std::unique_ptr<Detail::RenderSystem::NativeEffectPass> nativeEffectPass;
};

}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTPASS_3FA791E9_F535_41AF_B197_3DF71A12085A_HPP)
