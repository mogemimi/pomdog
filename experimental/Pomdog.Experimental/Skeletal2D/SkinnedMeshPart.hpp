// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SKINNEDMESHPART_DAE847CA_HPP
#define POMDOG_SKINNEDMESHPART_DAE847CA_HPP

#include <cstdint>

namespace Pomdog {

class SkinnedMeshPart {
public:
    std::uint16_t VertexCount;

    std::uint16_t VertexOffset;

    std::uint16_t PrimitiveCount;

    std::uint16_t StartIndex;
};

}// namespace Pomdog

#endif // POMDOG_SKINNEDMESHPART_DAE847CA_HPP
