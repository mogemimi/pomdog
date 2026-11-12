// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/mesh3d/mesh_data.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_hash32.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/mesh3d_debug_generated.h"
#include "pomdogschemas/mesh3d_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::computeStringHash32;
using pomdog::Error;
using pomdog::u16;
using pomdog::u32;
using pomdog::u8;
using pomdog::mesh3d::MeshData;
using pomdog::mesh3d::VertexSemantic;

namespace {

/// TemporaryFile writes bytes to a file in the temporary directory, and removes
/// the file when it goes out of scope.
class TemporaryFile final {
private:
    std::filesystem::path path_;
    std::string directory_;
    std::string name_;

public:
    explicit TemporaryFile(std::span<const u8> bytes)
    {
        // NOTE: A file stays open while its data is alive, so every file gets
        // its own name.
        static int serial = 0;
        name_ = "pomdog_mesh_data_test_" + std::to_string(serial) + ".mesh3d";
        ++serial;

        auto [tempDir, tempErr] = pomdog::platformfs::getTempDirectoryPath();
        REQUIRE(tempErr == nullptr);
        directory_ = tempDir;
        path_ = std::filesystem::path{tempDir} / name_;

        std::ofstream stream(path_, std::ios::binary | std::ios::trunc);
        REQUIRE(stream.is_open());
        stream.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
        REQUIRE(stream.good());
    }

    TemporaryFile(const TemporaryFile&) = delete;
    TemporaryFile& operator=(const TemporaryFile&) = delete;
    TemporaryFile(TemporaryFile&&) = delete;
    TemporaryFile& operator=(TemporaryFile&&) = delete;

    ~TemporaryFile()
    {
        std::error_code err;
        std::filesystem::remove(path_, err);
    }

    /// Returns a VFS that mounts the temporary directory at "/tmp".
    [[nodiscard]] std::shared_ptr<pomdog::vfs::FileSystemContext>
    mount() const
    {
        auto [ctx, ctxErr] = pomdog::vfs::create();
        REQUIRE(ctxErr == nullptr);
        REQUIRE(pomdog::vfs::mount(ctx, "/tmp", directory_, {.readOnly = true}) == nullptr);
        return std::move(ctx);
    }

    [[nodiscard]] std::string
    getPath() const
    {
        return "/tmp/" + name_;
    }
};

/// Loaded holds mesh data with the file that it reads. The data closes the
/// file before the file is removed, because members are destroyed in reverse
/// order.
struct Loaded final {
    std::unique_ptr<TemporaryFile> file;
    std::shared_ptr<MeshData> data;
    std::unique_ptr<Error> err;

    Loaded() = default;
    Loaded(const Loaded&) = delete;
    Loaded& operator=(const Loaded&) = delete;
    Loaded(Loaded&&) = default;
};

[[nodiscard]] std::vector<u8>
toBytes(const flatbuffers::FlatBufferBuilder& builder)
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    const auto bytes = std::span<const u8>{builder.GetBufferPointer(), builder.GetSize()};
    POMDOG_CLANG_UNSAFE_BUFFER_END
    return std::vector<u8>(bytes.begin(), bytes.end());
}

[[nodiscard]] Loaded
loadBytes(std::span<const u8> bytes)
{
    Loaded result;
    result.file = std::make_unique<TemporaryFile>(bytes);
    std::tie(result.data, result.err) = pomdog::mesh3d::loadMeshData(result.file->mount(), result.file->getPath());
    return result;
}

[[nodiscard]] Loaded
load(const pomdogschemas::Mesh3DT& mesh)
{
    flatbuffers::FlatBufferBuilder builder;
    builder.Finish(pomdogschemas::Mesh3D::Pack(builder, &mesh));
    return loadBytes(toBytes(builder));
}

[[nodiscard]] pomdogschemas::Mesh3DVertexElement
makeElement(u16 byteOffset, pomdogschemas::Mesh3DVertexSemantic semantic, u8 semanticIndex, pomdogschemas::Mesh3DVertexElementFormat format)
{
    return pomdogschemas::Mesh3DVertexElement{byteOffset, semantic, semanticIndex, format};
}

/// Returns two meshes. "body" has two parts, which draw 6 and 3 indices with
/// the vertex buffers 0 and 1, and "wing" has one part of 3 indices.
/// Vertex buffer 0 has 4 vertices of positions, normals, and texture
/// coordinates (32 bytes each), and vertex buffer 1 has 3 vertices of
/// positions and the second set of texture coordinates (20 bytes each).
[[nodiscard]] pomdogschemas::Mesh3DT
makeMeshes()
{
    auto buffer0 = std::make_unique<pomdogschemas::Mesh3DVertexBufferT>();
    buffer0->byte_offset = 0;
    buffer0->vertex_count = 4;
    buffer0->stride_bytes = 32;
    buffer0->elements.push_back(makeElement(0, pomdogschemas::Mesh3DVertexSemantic_Position, 0, pomdogschemas::Mesh3DVertexElementFormat_Float32x3));
    buffer0->elements.push_back(makeElement(12, pomdogschemas::Mesh3DVertexSemantic_Normal, 0, pomdogschemas::Mesh3DVertexElementFormat_Float32x3));
    buffer0->elements.push_back(makeElement(24, pomdogschemas::Mesh3DVertexSemantic_TexCoord, 0, pomdogschemas::Mesh3DVertexElementFormat_Float32x2));

    auto buffer1 = std::make_unique<pomdogschemas::Mesh3DVertexBufferT>();
    buffer1->byte_offset = 4 * 32;
    buffer1->vertex_count = 3;
    buffer1->stride_bytes = 20;
    buffer1->elements.push_back(makeElement(0, pomdogschemas::Mesh3DVertexSemantic_Position, 0, pomdogschemas::Mesh3DVertexElementFormat_Float32x3));
    buffer1->elements.push_back(makeElement(12, pomdogschemas::Mesh3DVertexSemantic_TexCoord, 1, pomdogschemas::Mesh3DVertexElementFormat_Float32x2));

    auto body = std::make_unique<pomdogschemas::Mesh3DMeshT>();
    body->parts.emplace_back(u32(0), u32(6), u16(0));
    body->parts.emplace_back(u32(6), u32(3), u16(1));

    auto wing = std::make_unique<pomdogschemas::Mesh3DMeshT>();
    wing->parts.emplace_back(u32(9), u32(3), u16(0));

    // NOTE: The keys must be in ascending order.
    std::vector<std::pair<u32, std::unique_ptr<pomdogschemas::Mesh3DMeshT>>> meshes;
    meshes.emplace_back(computeStringHash32("body"), std::move(body));
    meshes.emplace_back(computeStringHash32("wing"), std::move(wing));
    if (meshes[0].first > meshes[1].first) {
        std::swap(meshes[0], meshes[1]);
    }

    pomdogschemas::Mesh3DT mesh;
    for (auto& [key, value] : meshes) {
        mesh.keys.push_back(key);
        mesh.meshes.push_back(std::move(value));
    }
    mesh.vertex_buffers.push_back(std::move(buffer0));
    mesh.vertex_buffers.push_back(std::move(buffer1));
    mesh.index_count = 12;
    mesh.index_format = pomdogschemas::Mesh3DIndexFormat_UInt16;
    return mesh;
}

/// Returns the mesh "body" of the meshes that makeMeshes() returned.
[[nodiscard]] pomdogschemas::Mesh3DMeshT&
getBody(pomdogschemas::Mesh3DT& mesh)
{
    if (mesh.keys[0] == computeStringHash32("body")) {
        return *mesh.meshes[0];
    }
    return *mesh.meshes[1];
}

} // namespace

TEST_CASE("loadMeshData")
{
    SUBCASE("finds a mesh by the hash of its name")
    {
        const auto loaded = load(makeMeshes());
        REQUIRE(loaded.err == nullptr);
        REQUIRE(loaded.data != nullptr);
        const auto& data = *loaded.data;

        REQUIRE(data.getMeshCount() == 2);
        REQUIRE_FALSE(data.findMeshByKey(computeStringHash32("missing")).has_value());

        const auto body = data.findMeshByKey(computeStringHash32("body"));
        REQUIRE(body.has_value());
        REQUIRE(data.getPartCount(*body) == 2);
        REQUIRE(data.getPart(*body, 0).startIndex == 0);
        REQUIRE(data.getPart(*body, 0).indexCount == 6);
        REQUIRE(data.getPart(*body, 0).vertexBuffer == 0);
        REQUIRE(data.getPart(*body, 1).startIndex == 6);
        REQUIRE(data.getPart(*body, 1).indexCount == 3);
        REQUIRE(data.getPart(*body, 1).vertexBuffer == 1);

        const auto wing = data.findMeshByKey(computeStringHash32("wing"));
        REQUIRE(wing.has_value());
        REQUIRE(*wing != *body);
        REQUIRE(data.getPartCount(*wing) == 1);
        REQUIRE(data.getPart(*wing, 0).startIndex == 9);
        REQUIRE(data.getPart(*wing, 0).indexCount == 3);
    }
    SUBCASE("describes the vertex buffers and the sizes of the raw files")
    {
        const auto loaded = load(makeMeshes());
        REQUIRE(loaded.err == nullptr);
        const auto& data = *loaded.data;

        REQUIRE(data.getVertexBufferCount() == 2);

        const auto layout0 = data.getVertexBufferLayout(0);
        REQUIRE(layout0.byteOffset == 0);
        REQUIRE(layout0.vertexCount == 4);
        REQUIRE(layout0.strideBytes == 32);
        REQUIRE(layout0.elementCount == 3);

        const auto normal = data.getVertexElement(0, 1);
        REQUIRE(normal.byteOffset == 12);
        REQUIRE((normal.format == pomdog::gpu::InputElementFormat::Float32x3));
        REQUIRE((normal.semantic == VertexSemantic::Normal));
        REQUIRE(normal.semanticIndex == 0);

        const auto layout1 = data.getVertexBufferLayout(1);
        REQUIRE(layout1.byteOffset == 128);
        REQUIRE(layout1.vertexCount == 3);
        REQUIRE(layout1.strideBytes == 20);
        REQUIRE(layout1.elementCount == 2);

        const auto texCoord = data.getVertexElement(1, 1);
        REQUIRE(texCoord.byteOffset == 12);
        REQUIRE((texCoord.format == pomdog::gpu::InputElementFormat::Float32x2));
        REQUIRE((texCoord.semantic == VertexSemantic::TexCoord));
        REQUIRE(texCoord.semanticIndex == 1);

        REQUIRE((data.getIndexFormat() == pomdog::gpu::IndexFormat::UInt16));
        REQUIRE(data.getIndexCount() == 12);
        REQUIRE(data.getVerticesByteLength() == 4 * 32 + 3 * 20);
        REQUIRE(data.getIndicesByteLength() == 12 * 2);
    }
    SUBCASE("sizes 32-bit indices")
    {
        auto mesh = makeMeshes();
        mesh.index_format = pomdogschemas::Mesh3DIndexFormat_UInt32;

        const auto loaded = load(mesh);
        REQUIRE(loaded.err == nullptr);
        REQUIRE((loaded.data->getIndexFormat() == pomdog::gpu::IndexFormat::UInt32));
        REQUIRE(loaded.data->getIndicesByteLength() == 12 * 4);
    }
    SUBCASE("fails for a missing file")
    {
        auto [ctx, ctxErr] = pomdog::vfs::create();
        REQUIRE(ctxErr == nullptr);

        auto [data, err] = pomdog::mesh3d::loadMeshData(ctx, "/tmp/pomdog_mesh_data_test_missing.mesh3d");
        REQUIRE(data == nullptr);
        REQUIRE(err != nullptr);
    }
    SUBCASE("rejects truncated bytes")
    {
        flatbuffers::FlatBufferBuilder builder;
        const auto mesh = makeMeshes();
        builder.Finish(pomdogschemas::Mesh3D::Pack(builder, &mesh));
        auto bytes = toBytes(builder);
        bytes.resize(bytes.size() / 2);

        const auto loaded = loadBytes(bytes);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a file without vertex buffers")
    {
        auto mesh = makeMeshes();
        mesh.vertex_buffers.clear();

        const auto loaded = load(mesh);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a file without indices")
    {
        auto mesh = makeMeshes();
        mesh.index_count = 0;

        const auto loaded = load(mesh);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects an unknown index format")
    {
        auto mesh = makeMeshes();
        mesh.index_format = static_cast<pomdogschemas::Mesh3DIndexFormat>(2);

        const auto loaded = load(mesh);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects keys that are not in ascending order")
    {
        auto mesh = makeMeshes();
        std::swap(mesh.keys[0], mesh.keys[1]);

        const auto loaded = load(mesh);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects keys and meshes of different lengths")
    {
        auto mesh = makeMeshes();
        mesh.keys.pop_back();

        const auto loaded = load(mesh);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a part whose vertex buffer is out of range")
    {
        auto mesh = makeMeshes();
        getBody(mesh).parts[1] = pomdogschemas::Mesh3DPart{u32(6), u32(3), u16(2)};

        const auto loaded = load(mesh);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a part whose indices are out of range")
    {
        auto mesh = makeMeshes();
        getBody(mesh).parts[1] = pomdogschemas::Mesh3DPart{u32(12), u32(3), u16(1)};

        const auto loaded = load(mesh);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a part whose indices do not form triangles")
    {
        auto mesh = makeMeshes();
        getBody(mesh).parts[1] = pomdogschemas::Mesh3DPart{u32(6), u32(4), u16(1)};

        const auto loaded = load(mesh);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a vertex buffer that does not follow the previous one")
    {
        auto mesh = makeMeshes();
        mesh.vertex_buffers[1]->byte_offset = 4 * 32 + 4;

        const auto loaded = load(mesh);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a vertex buffer without vertices")
    {
        auto mesh = makeMeshes();
        mesh.vertex_buffers[1]->vertex_count = 0;

        const auto loaded = load(mesh);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a stride that differs from the size of the elements")
    {
        auto mesh = makeMeshes();
        mesh.vertex_buffers[0]->stride_bytes = 36;
        mesh.vertex_buffers[1]->byte_offset = 4 * 36;

        const auto loaded = load(mesh);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects elements with a gap between them")
    {
        auto mesh = makeMeshes();
        mesh.vertex_buffers[1]->elements[1] = makeElement(16, pomdogschemas::Mesh3DVertexSemantic_TexCoord, 1, pomdogschemas::Mesh3DVertexElementFormat_Float32x1);

        const auto loaded = load(mesh);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects an element of an unknown format or semantic")
    {
        auto mesh = makeMeshes();
        mesh.vertex_buffers[1]->elements[1] = makeElement(12, pomdogschemas::Mesh3DVertexSemantic_TexCoord, 1, static_cast<pomdogschemas::Mesh3DVertexElementFormat>(4));
        REQUIRE(load(mesh).err != nullptr);

        mesh.vertex_buffers[1]->elements[1] = makeElement(12, static_cast<pomdogschemas::Mesh3DVertexSemantic>(7), 1, pomdogschemas::Mesh3DVertexElementFormat_Float32x2);
        REQUIRE(load(mesh).err != nullptr);
    }
}
