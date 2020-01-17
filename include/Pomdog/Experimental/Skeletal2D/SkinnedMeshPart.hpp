// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog::Skeletal2D {

class SkinnedMeshPart final {
public:
    std::uint16_t VertexCount;

    std::uint16_t VertexOffset;

    std::uint16_t PrimitiveCount;

    std::uint16_t StartIndex;
};

} // namespace Pomdog::Skeletal2D
