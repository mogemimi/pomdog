// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/shader_reflections/effect_variable.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

struct EffectConstantDescription final {
    std::vector<EffectVariable> Variables;
    std::string Name;
    std::uint32_t ByteSize;
};

} // namespace pomdog::gpu
