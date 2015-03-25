// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_NATIVEEFFECTREFLECTION_F923B192_157E_4D4E_BBBF_300ABEA0485A_HPP
#define POMDOG_NATIVEEFFECTREFLECTION_F923B192_157E_4D4E_BBBF_300ABEA0485A_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vector>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeEffectReflection {
public:
    NativeEffectReflection() = default;
    NativeEffectReflection(NativeEffectReflection const&) = delete;
    NativeEffectReflection & operator=(NativeEffectReflection const&) = delete;

    virtual ~NativeEffectReflection() = default;

    virtual std::vector<EffectConstantDescription> GetConstantBuffers() const = 0;
};

}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_NATIVEEFFECTREFLECTION_F923B192_157E_4D4E_BBBF_300ABEA0485A_HPP)
