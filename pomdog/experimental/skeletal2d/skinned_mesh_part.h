// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

namespace pomdog::skeletal2d {

class SkinnedMeshPart final {
public:
    u16 vertexCount;

    u16 vertexOffset;

    u16 primitiveCount;

    u16 startIndex;
};

} // namespace pomdog::skeletal2d
