// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string_view>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::gpu {
class GraphicsDevice;
class IndexBuffer;
class VertexBuffer;
} // namespace pomdog::gpu

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog::mesh3d {

class MeshData;

/// MeshBuffers holds the GPU buffers of the meshes of a `.mesh3d` file.
struct MeshBuffers final {
    /// One buffer for each vertex buffer of the mesh file, in the same order.
    std::vector<std::shared_ptr<gpu::VertexBuffer>> vertexBuffers;

    /// The indices of every part of the mesh file.
    std::shared_ptr<gpu::IndexBuffer> indexBuffer;
};

/// Loads the vertices file (`.mesh3dvertices`) and the indices file
/// (`.mesh3dindices`) of a mesh file into immutable GPU buffers.
///
/// The files have no header, and their bytes go to the GPU as they are. Both
/// files must come from the same conversion as `meshData`; a file whose size
/// differs from what `meshData` expects is an error.
[[nodiscard]] POMDOG_EXPORT std::tuple<MeshBuffers, std::unique_ptr<Error>>
loadMeshBuffers(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const MeshData& meshData,
    std::string_view verticesFilePath,
    std::string_view indicesFilePath) noexcept;

} // namespace pomdog::mesh3d
