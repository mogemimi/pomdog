// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/input_element_format.h"

namespace pomdog::mesh3d {

/// VertexSemantic tells what an element of a vertex holds.
enum class VertexSemantic : u8 {
    Position = 0,
    Normal = 1,
    Tangent = 2,
    TexCoord = 3,
    Color = 4,
    Joints = 5,
    Weights = 6,
};

/// VertexElement describes one element of a vertex.
struct VertexElement final {
    /// Byte offset from the beginning of the vertex.
    u32 byteOffset = 0;

    /// Data format of the element.
    gpu::InputElementFormat format = gpu::InputElementFormat::Float32x1;

    /// What the element holds.
    VertexSemantic semantic = VertexSemantic::Position;

    /// Tells elements of the same semantic apart, such as the second set of
    /// texture coordinates.
    u8 semanticIndex = 0;
};

/// VertexBufferLayout describes a vertex buffer of a mesh file: a range of the
/// vertices file whose vertices share a layout.
struct VertexBufferLayout final {
    /// Byte offset of the first vertex in the vertices file.
    u32 byteOffset = 0;

    /// The number of vertices.
    u32 vertexCount = 0;

    /// Size in bytes of a vertex.
    u32 strideBytes = 0;

    /// The number of elements of a vertex.
    u32 elementCount = 0;
};

} // namespace pomdog::mesh3d
