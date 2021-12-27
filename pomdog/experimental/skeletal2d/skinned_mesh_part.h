// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {

class SkinnedMeshPart final {
public:
    std::uint16_t VertexCount;

    std::uint16_t VertexOffset;

    std::uint16_t PrimitiveCount;

    std::uint16_t StartIndex;
};

} // namespace pomdog::skeletal2d
