// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Skeletal2D {

class SkinnedMeshPart final {
public:
    std::uint16_t VertexCount;

    std::uint16_t VertexOffset;

    std::uint16_t PrimitiveCount;

    std::uint16_t StartIndex;
};

} // namespace Pomdog::Skeletal2D
