// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_NATIVEEFFECTREFLECTION_F923B192_HPP
#define POMDOG_NATIVEEFFECTREFLECTION_F923B192_HPP

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vector>

namespace Pomdog {
namespace Detail {

class NativeEffectReflection {
public:
    NativeEffectReflection() = default;
    NativeEffectReflection(NativeEffectReflection const&) = delete;
    NativeEffectReflection & operator=(NativeEffectReflection const&) = delete;

    virtual ~NativeEffectReflection() = default;

    virtual std::vector<EffectConstantDescription> GetConstantBuffers() const = 0;
};

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_NATIVEEFFECTREFLECTION_F923B192_HPP
