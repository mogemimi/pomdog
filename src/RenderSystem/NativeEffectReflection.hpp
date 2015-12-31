// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

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
