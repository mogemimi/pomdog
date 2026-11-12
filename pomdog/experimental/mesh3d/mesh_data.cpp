// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/mesh3d/mesh_data.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/mesh3d_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <iterator>
#include <limits>
#include <string>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::mesh3d {
namespace {

constexpr u32 bytesPerFloat = 4;

// NOTE: The formats of the file are numbered like the float formats of the GPU.
static_assert(static_cast<u8>(gpu::InputElementFormat::Float32x1) == pomdogschemas::Mesh3DVertexElementFormat_Float32x1);
static_assert(static_cast<u8>(gpu::InputElementFormat::Float32x2) == pomdogschemas::Mesh3DVertexElementFormat_Float32x2);
static_assert(static_cast<u8>(gpu::InputElementFormat::Float32x3) == pomdogschemas::Mesh3DVertexElementFormat_Float32x3);
static_assert(static_cast<u8>(gpu::InputElementFormat::Float32x4) == pomdogschemas::Mesh3DVertexElementFormat_Float32x4);

// NOTE: The semantics of the file have the values of VertexSemantic.
static_assert(static_cast<u8>(VertexSemantic::Position) == pomdogschemas::Mesh3DVertexSemantic_Position);
static_assert(static_cast<u8>(VertexSemantic::Weights) == pomdogschemas::Mesh3DVertexSemantic_Weights);

template <typename T>
[[nodiscard]] u32
getSize(const flatbuffers::Vector<T>* vector) noexcept
{
    if (vector == nullptr) {
        return 0;
    }
    return vector->size();
}

[[nodiscard]] u32
getIndexSize(pomdogschemas::Mesh3DIndexFormat format) noexcept
{
    if (format == pomdogschemas::Mesh3DIndexFormat_UInt32) {
        return 4;
    }
    return 2;
}

/// Verifies a vertex buffer that starts at `byteOffset` of the vertices file.
[[nodiscard]] std::unique_ptr<Error>
validateVertexBuffer(const pomdogschemas::Mesh3DVertexBuffer& buffer, u64 byteOffset) noexcept
{
    // NOTE: The buffers follow one another, so that the vertices file has no
    // bytes that the mesh file does not describe.
    if (buffer.byte_offset() != byteOffset) {
        return errors::make(pomdog::format("the buffer starts at byte {}, but the buffers before it end at byte {}", buffer.byte_offset(), byteOffset));
    }
    if (buffer.vertex_count() == 0) {
        return errors::make("the buffer has no vertices");
    }

    const auto* elements = buffer.elements();
    if (getSize(elements) == 0) {
        return errors::make("the vertices have no elements");
    }

    // NOTE: The elements fill the vertex without gaps.
    u32 offset = 0;
    for (const auto* element : *elements) {
        if (element->format() > pomdogschemas::Mesh3DVertexElementFormat_MAX) {
            return errors::make("an element has an unknown format");
        }
        if (element->semantic() > pomdogschemas::Mesh3DVertexSemantic_MAX) {
            return errors::make("an element has an unknown semantic");
        }
        if (element->byte_offset() != offset) {
            return errors::make(pomdog::format("an element starts at byte {}, but the elements before it end at byte {}", element->byte_offset(), offset));
        }
        const auto componentCount = static_cast<u32>(element->format()) + 1;
        offset += componentCount * bytesPerFloat;
    }
    if (buffer.stride_bytes() != offset) {
        return errors::make(pomdog::format("a vertex takes {} bytes, but its elements take {}", buffer.stride_bytes(), offset));
    }
    return nullptr;
}

/// Verifies the table, and returns the sizes that the vertices file and the
/// indices file must have.
[[nodiscard]] std::tuple<std::size_t, std::size_t, std::unique_ptr<Error>>
validateMeshes(const pomdogschemas::Mesh3D& table) noexcept
{
    if (table.index_format() > pomdogschemas::Mesh3DIndexFormat_MAX) {
        return {0, 0, errors::make("the indices have an unknown format")};
    }
    if (table.index_count() == 0) {
        return {0, 0, errors::make("the file has no indices")};
    }

    const auto* vertexBuffers = table.vertex_buffers();
    const auto vertexBufferCount = getSize(vertexBuffers);
    if (vertexBufferCount == 0) {
        return {0, 0, errors::make("the file has no vertex buffers")};
    }

    u64 verticesByteLength = 0;
    for (u32 index = 0; index < vertexBufferCount; ++index) {
        const auto* buffer = vertexBuffers->Get(index);
        if (auto err = validateVertexBuffer(*buffer, verticesByteLength); err != nullptr) {
            return {0, 0, errors::wrap(std::move(err), pomdog::format("invalid vertex buffer {}", index))};
        }
        verticesByteLength += u64{buffer->vertex_count()} * buffer->stride_bytes();
    }

    // NOTE: A byte offset in the vertices file has 32 bits.
    if (verticesByteLength > std::numeric_limits<u32>::max()) {
        return {0, 0, errors::make("the vertices take more bytes than a 32-bit offset can address")};
    }
    const auto indicesByteLength = u64{table.index_count()} * getIndexSize(table.index_format());
    if (indicesByteLength > std::numeric_limits<u32>::max()) {
        return {0, 0, errors::make("the indices take more bytes than a 32-bit offset can address")};
    }

    const auto* keys = table.keys();
    const auto* meshes = table.meshes();
    const auto meshCount = getSize(meshes);
    if (getSize(keys) != meshCount) {
        return {0, 0, errors::make("keys and meshes must have the same length")};
    }

    for (u32 index = 0; index < meshCount; ++index) {
        // NOTE: findMeshByKey() searches the keys with a binary search.
        if (index > 0 && keys->Get(index - 1) >= keys->Get(index)) {
            return {0, 0, errors::make("keys must be in ascending order without duplicates")};
        }

        const auto* parts = meshes->Get(index)->parts();
        if (parts == nullptr) {
            continue;
        }
        for (const auto* part : *parts) {
            if (part->vertex_buffer() >= vertexBufferCount) {
                return {0, 0, errors::make(pomdog::format("mesh {} has a part whose vertex buffer {} is out of the {} vertex buffers", index, part->vertex_buffer(), vertexBufferCount))};
            }
            if (part->index_count() % 3 != 0) {
                return {0, 0, errors::make(pomdog::format("mesh {} has a part of {} indices, but triangles need three indices each", index, part->index_count()))};
            }
            if (part->start_index() > table.index_count() || part->index_count() > table.index_count() - part->start_index()) {
                return {0, 0, errors::make(pomdog::format("mesh {} has a part whose indices are out of the {} indices", index, table.index_count()))};
            }
        }
    }

    return {static_cast<std::size_t>(verticesByteLength), static_cast<std::size_t>(indicesByteLength), nullptr};
}

} // namespace

MeshData::MeshData(
    std::shared_ptr<vfs::File> file,
    const pomdogschemas::Mesh3D* table) noexcept
    : file_(std::move(file))
    , table_(table)
{
    POMDOG_ASSERT(file_ != nullptr);
    POMDOG_ASSERT(table_ != nullptr);

    // NOTE: loadMeshData() has verified that the sizes fit.
    for (const auto* buffer : *table_->vertex_buffers()) {
        verticesByteLength_ += static_cast<std::size_t>(buffer->vertex_count()) * buffer->stride_bytes();
    }
    indicesByteLength_ = static_cast<std::size_t>(table_->index_count()) * getIndexSize(table_->index_format());
}

u32 MeshData::getMeshCount() const noexcept
{
    return getSize(table_->meshes());
}

std::optional<u32>
MeshData::findMeshByKey(u32 key) const noexcept
{
    const auto* keys = table_->keys();
    if (keys == nullptr) {
        return std::nullopt;
    }

    const auto iter = std::lower_bound(keys->begin(), keys->end(), key);
    if (iter == keys->end() || *iter != key) {
        return std::nullopt;
    }
    return static_cast<u32>(std::distance(keys->begin(), iter));
}

u32 MeshData::getPartCount(u32 meshIndex) const noexcept
{
    POMDOG_ASSERT(meshIndex < getMeshCount());
    return getSize(table_->meshes()->Get(meshIndex)->parts());
}

MeshPart
MeshData::getPart(u32 meshIndex, u32 partIndex) const noexcept
{
    POMDOG_ASSERT(partIndex < getPartCount(meshIndex));
    const auto* part = table_->meshes()->Get(meshIndex)->parts()->Get(partIndex);

    MeshPart result;
    result.startIndex = part->start_index();
    result.indexCount = part->index_count();
    result.vertexBuffer = part->vertex_buffer();
    return result;
}

u32 MeshData::getVertexBufferCount() const noexcept
{
    return getSize(table_->vertex_buffers());
}

VertexBufferLayout
MeshData::getVertexBufferLayout(u32 vertexBufferIndex) const noexcept
{
    POMDOG_ASSERT(vertexBufferIndex < getVertexBufferCount());
    const auto* buffer = table_->vertex_buffers()->Get(vertexBufferIndex);

    VertexBufferLayout result;
    result.byteOffset = buffer->byte_offset();
    result.vertexCount = buffer->vertex_count();
    result.strideBytes = buffer->stride_bytes();
    result.elementCount = getSize(buffer->elements());
    return result;
}

VertexElement
MeshData::getVertexElement(u32 vertexBufferIndex, u32 elementIndex) const noexcept
{
    POMDOG_ASSERT(vertexBufferIndex < getVertexBufferCount());
    const auto* elements = table_->vertex_buffers()->Get(vertexBufferIndex)->elements();
    POMDOG_ASSERT(elementIndex < getSize(elements));
    const auto* element = elements->Get(elementIndex);

    VertexElement result;
    result.byteOffset = element->byte_offset();
    result.format = static_cast<gpu::InputElementFormat>(element->format());
    result.semantic = static_cast<VertexSemantic>(element->semantic());
    result.semanticIndex = element->semantic_index();
    return result;
}

gpu::IndexFormat
MeshData::getIndexFormat() const noexcept
{
    if (table_->index_format() == pomdogschemas::Mesh3DIndexFormat_UInt32) {
        return gpu::IndexFormat::UInt32;
    }
    return gpu::IndexFormat::UInt16;
}

u32 MeshData::getIndexCount() const noexcept
{
    return table_->index_count();
}

std::size_t
MeshData::getVerticesByteLength() const noexcept
{
    return verticesByteLength_;
}

std::size_t
MeshData::getIndicesByteLength() const noexcept
{
    return indicesByteLength_;
}

std::tuple<std::shared_ptr<MeshData>, std::unique_ptr<Error>>
loadMeshData(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    std::string_view filePath) noexcept
{
    const auto path = std::string{filePath};

    auto [file, openErr] = vfs::open(fs, path);
    if (openErr != nullptr) {
        return {nullptr, errors::wrap(std::move(openErr), "cannot open mesh file, " + path)};
    }

    auto [bytes, viewErr] = file->view();
    if (viewErr != nullptr) {
        return {nullptr, errors::wrap(std::move(viewErr), "cannot read mesh file, " + path)};
    }

    flatbuffers::Verifier verifier(bytes.data(), bytes.size());
    if (!pomdogschemas::VerifyMesh3DBuffer(verifier)) {
        return {nullptr, errors::make("FlatBuffers verification failed for mesh file, " + path)};
    }

    const auto* table = pomdogschemas::GetMesh3D(bytes.data());
    if (auto [verticesByteLength, indicesByteLength, err] = validateMeshes(*table); err != nullptr) {
        return {nullptr, errors::wrap(std::move(err), "invalid mesh file, " + path)};
    }

    return {std::make_shared<MeshData>(std::move(file), table), nullptr};
}

} // namespace pomdog::mesh3d
