// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_NATIVEEFFECTPASS_F0C96CAC_A22D_4E8F_A31F_5C2E7A91C273_HPP
#define POMDOG_NATIVEEFFECTPASS_F0C96CAC_A22D_4E8F_A31F_5C2E7A91C273_HPP

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>
#include <string>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeConstantLayout;
class NativeGraphicsDevice;

class NativeEffectPass {
public:
    NativeEffectPass() = default;
    NativeEffectPass(NativeEffectPass const&) = delete;
    NativeEffectPass & operator=(NativeEffectPass const&) = delete;

    virtual ~NativeEffectPass() = default;

    virtual std::unique_ptr<NativeConstantLayout> CreateConstantLayout() = 0;
};

}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_NATIVEEFFECTPASS_F0C96CAC_A22D_4E8F_A31F_5C2E7A91C273_HPP)
