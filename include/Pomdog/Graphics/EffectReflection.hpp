// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/EffectConstantDescription.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include <memory>
#include <vector>

namespace Pomdog {

class POMDOG_EXPORT EffectReflection {
public:
    EffectReflection() noexcept;
    EffectReflection(const EffectReflection&) = delete;
    EffectReflection(EffectReflection&&) = default;

    virtual ~EffectReflection();

    [[nodiscard]] virtual std::vector<EffectConstantDescription>
    GetConstantBuffers() const noexcept = 0;
};

} // namespace Pomdog
