// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/shader_reflections/effect_constant_description.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

class POMDOG_EXPORT EffectReflection {
public:
    EffectReflection() noexcept;
    EffectReflection(const EffectReflection&) = delete;
    EffectReflection(EffectReflection&&) = default;

    virtual ~EffectReflection();

    [[nodiscard]] virtual std::vector<EffectConstantDescription>
    getConstantBuffers() const noexcept = 0;
};

} // namespace pomdog::gpu
