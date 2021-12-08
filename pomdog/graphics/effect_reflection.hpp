// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/graphics/effect_constant_description.hpp"
#include "pomdog/graphics/forward_declarations.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class POMDOG_EXPORT EffectReflection {
public:
    EffectReflection() noexcept;
    EffectReflection(const EffectReflection&) = delete;
    EffectReflection(EffectReflection&&) = default;

    virtual ~EffectReflection();

    [[nodiscard]] virtual std::vector<EffectConstantDescription>
    GetConstantBuffers() const noexcept = 0;
};

} // namespace pomdog
