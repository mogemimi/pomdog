// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail {

struct GraphicsCapabilities final {
    std::size_t ConstantBufferSlotCount;
    std::size_t SamplerSlotCount;
};

} // namespace pomdog::gpu::detail
