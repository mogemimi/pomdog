// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::mesh3d {

/// MeshPart is the range of an index buffer that one draw call draws as a
/// list of triangles.
struct MeshPart final {
    /// The position of the first index in the index buffer.
    u32 startIndex = 0;

    /// The number of indices, three for each triangle.
    u32 indexCount = 0;

    /// The index of the vertex buffer that the indices refer to.
    u32 vertexBuffer = 0;
};

} // namespace pomdog::mesh3d
