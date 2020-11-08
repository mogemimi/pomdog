// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include <vector>

namespace Pomdog::Detail {

class NativeEffectReflection {
public:
    NativeEffectReflection() = default;
    NativeEffectReflection(const NativeEffectReflection&) = delete;
    NativeEffectReflection& operator=(const NativeEffectReflection&) = delete;

    virtual ~NativeEffectReflection() = default;

    virtual std::vector<EffectConstantDescription> GetConstantBuffers() const = 0;
};

} // namespace Pomdog::Detail
