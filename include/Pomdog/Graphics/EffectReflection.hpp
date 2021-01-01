// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/EffectConstantDescription.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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
