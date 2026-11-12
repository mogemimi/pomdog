// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/mesh3d/mesh_part.h"
#include "pomdog/experimental/mesh3d/vertex_layout.h"
#include "pomdog/gpu/index_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <memory>
#include <optional>
#include <string_view>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::vfs {
class File;
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdogschemas {
struct Mesh3D;
} // namespace pomdogschemas

namespace pomdog::mesh3d {

/// MeshData is the meshes of a `.mesh3d` file, which `mesh3d-source-to-mesh3d`
/// writes offline from a model such as a glTF file.
///
/// The file is small and has no vertex data, so an application can keep it in
/// memory. It tells how to draw the meshes with the GPU buffers that
/// `loadMeshBuffers()` loads from the vertices file and the indices file of
/// the same conversion.
///
/// MeshData keeps the file open and reads the FlatBuffers table from its
/// view, so that a memory-mapped archive serves it without copying it.
class POMDOG_EXPORT MeshData final {
private:
    std::shared_ptr<vfs::File> file_;
    const pomdogschemas::Mesh3D* table_ = nullptr;
    std::size_t verticesByteLength_ = 0;
    std::size_t indicesByteLength_ = 0;

public:
    /// Takes a table in the view of `file` that `loadMeshData()` has verified.
    MeshData(
        std::shared_ptr<vfs::File> file,
        const pomdogschemas::Mesh3D* table) noexcept;

    /// Returns the number of meshes.
    [[nodiscard]] u32
    getMeshCount() const noexcept;

    /// Returns the index of the mesh whose name has the given xxHash32 key, or
    /// nullopt when the file has no such mesh.
    [[nodiscard]] std::optional<u32>
    findMeshByKey(u32 key) const noexcept;

    /// Returns the number of parts of the mesh at the given index.
    [[nodiscard]] u32
    getPartCount(u32 meshIndex) const noexcept;

    /// Returns a part of the mesh at the given index. Its indices are within
    /// the index buffer, and its vertex buffer is one of the vertex buffers.
    [[nodiscard]] MeshPart
    getPart(u32 meshIndex, u32 partIndex) const noexcept;

    /// Returns the number of vertex buffers.
    [[nodiscard]] u32
    getVertexBufferCount() const noexcept;

    /// Returns the layout of the vertex buffer at the given index.
    [[nodiscard]] VertexBufferLayout
    getVertexBufferLayout(u32 vertexBufferIndex) const noexcept;

    /// Returns an element of the vertices of the vertex buffer at the given
    /// index. The elements are in the order of their offsets and fill the
    /// vertex without gaps.
    [[nodiscard]] VertexElement
    getVertexElement(u32 vertexBufferIndex, u32 elementIndex) const noexcept;

    /// Returns the type of the indices.
    [[nodiscard]] gpu::IndexFormat
    getIndexFormat() const noexcept;

    /// Returns the number of indices in the indices file.
    [[nodiscard]] u32
    getIndexCount() const noexcept;

    /// Returns the size in bytes that the vertices file must have.
    [[nodiscard]] std::size_t
    getVerticesByteLength() const noexcept;

    /// Returns the size in bytes that the indices file must have.
    [[nodiscard]] std::size_t
    getIndicesByteLength() const noexcept;
};

/// Loads a `.mesh3d` file and verifies its content.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<MeshData>, std::unique_ptr<Error>>
loadMeshData(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    std::string_view filePath) noexcept;

} // namespace pomdog::mesh3d
