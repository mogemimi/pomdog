// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

class SkinnedMeshPart {
public:
    std::uint16_t VertexCount;

    std::uint16_t VertexOffset;

    std::uint16_t PrimitiveCount;

    std::uint16_t StartIndex;
};

} // namespace Pomdog
