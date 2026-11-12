// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/mesh3d/mesh_buffers.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/mesh3d/mesh_data.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/index_buffer.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <span>
#include <string>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::mesh3d {

std::tuple<MeshBuffers, std::unique_ptr<Error>>
loadMeshBuffers(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const MeshData& meshData,
    std::string_view verticesFilePath,
    std::string_view indicesFilePath) noexcept
{
    MeshBuffers buffers;

    {
        const auto path = std::string{verticesFilePath};

        auto [file, openErr] = vfs::open(fs, path);
        if (openErr != nullptr) {
            return {MeshBuffers{}, errors::wrap(std::move(openErr), "cannot open vertices file, " + path)};
        }

        auto [bytes, viewErr] = file->view();
        if (viewErr != nullptr) {
            return {MeshBuffers{}, errors::wrap(std::move(viewErr), "cannot read vertices file, " + path)};
        }

        // NOTE: The file has no header to check, so its size tells whether
        // it belongs to the mesh file.
        if (bytes.size() != meshData.getVerticesByteLength()) {
            return {MeshBuffers{}, errors::make(pomdog::format("vertices file has {} bytes, but the mesh file expects {}, {}", bytes.size(), meshData.getVerticesByteLength(), path))};
        }

        const auto vertexBufferCount = meshData.getVertexBufferCount();
        buffers.vertexBuffers.reserve(vertexBufferCount);

        for (u32 index = 0; index < vertexBufferCount; ++index) {
            const auto layout = meshData.getVertexBufferLayout(index);
            const auto vertices = bytes.subspan(layout.byteOffset, static_cast<std::size_t>(layout.vertexCount) * layout.strideBytes);

            auto [vertexBuffer, err] = graphicsDevice->createVertexBuffer(
                vertices.data(),
                layout.vertexCount,
                layout.strideBytes,
                gpu::BufferUsage::Immutable);
            if (err != nullptr) {
                return {MeshBuffers{}, errors::wrap(std::move(err), pomdog::format("failed to create vertex buffer {} of {}", index, path))};
            }
            buffers.vertexBuffers.push_back(std::move(vertexBuffer));
        }
    }
    {
        const auto path = std::string{indicesFilePath};

        auto [file, openErr] = vfs::open(fs, path);
        if (openErr != nullptr) {
            return {MeshBuffers{}, errors::wrap(std::move(openErr), "cannot open indices file, " + path)};
        }

        auto [bytes, viewErr] = file->view();
        if (viewErr != nullptr) {
            return {MeshBuffers{}, errors::wrap(std::move(viewErr), "cannot read indices file, " + path)};
        }

        if (bytes.size() != meshData.getIndicesByteLength()) {
            return {MeshBuffers{}, errors::make(pomdog::format("indices file has {} bytes, but the mesh file expects {}, {}", bytes.size(), meshData.getIndicesByteLength(), path))};
        }

        auto [indexBuffer, err] = graphicsDevice->createIndexBuffer(
            meshData.getIndexFormat(),
            bytes.data(),
            meshData.getIndexCount(),
            gpu::BufferUsage::Immutable);
        if (err != nullptr) {
            return {MeshBuffers{}, errors::wrap(std::move(err), "failed to create index buffer of " + path)};
        }
        buffers.indexBuffer = std::move(indexBuffer);
    }

    return {std::move(buffers), nullptr};
}

} // namespace pomdog::mesh3d
