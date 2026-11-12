// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/voxelmodel/voxel_model_writer.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/experimental/voxelmodel/voxel_model.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/voxel_model_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <span>
#include <string>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::voxelmodel {
namespace {

using AttributeVectorOffset = flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<pomdogschemas::VoxelModelAttribute>>>;

// NOTE: The writer leaves out empty strings and vectors, like the Go tools.

[[nodiscard]] flatbuffers::Offset<flatbuffers::String>
createString(flatbuffers::FlatBufferBuilder& builder, const std::string& s)
{
    if (s.empty()) {
        return 0;
    }
    return builder.CreateString(s);
}

/// Creates a string that the builder stores once however many times it
/// appears, as the keys and the values of materials repeat.
[[nodiscard]] flatbuffers::Offset<flatbuffers::String>
createSharedString(flatbuffers::FlatBufferBuilder& builder, const std::string& s)
{
    if (s.empty()) {
        return 0;
    }
    return builder.CreateSharedString(s);
}

[[nodiscard]] AttributeVectorOffset
createAttributes(flatbuffers::FlatBufferBuilder& builder, const std::vector<Attribute>& attributes)
{
    if (attributes.empty()) {
        return 0;
    }
    std::vector<flatbuffers::Offset<pomdogschemas::VoxelModelAttribute>> offsets;
    offsets.reserve(attributes.size());
    for (const auto& attribute : attributes) {
        const auto key = createSharedString(builder, attribute.key);
        const auto value = createSharedString(builder, attribute.value);
        offsets.push_back(pomdogschemas::CreateVoxelModelAttribute(builder, key, value));
    }
    return builder.CreateVector(offsets);
}

template <typename T>
[[nodiscard]] flatbuffers::Offset<flatbuffers::Vector<T>>
createVector(flatbuffers::FlatBufferBuilder& builder, const std::vector<T>& values)
{
    if (values.empty()) {
        return 0;
    }
    return builder.CreateVector(values);
}

[[nodiscard]] flatbuffers::Offset<pomdogschemas::VoxelModelNode>
createNode(flatbuffers::FlatBufferBuilder& builder, const Node& node)
{
    std::vector<flatbuffers::Offset<pomdogschemas::VoxelModelTransformFrame>> frames;
    frames.reserve(node.frames.size());
    for (const auto& frame : node.frames) {
        const auto attributes = createAttributes(builder, frame.attributes);
        const auto& t = frame.translation;
        const pomdogschemas::VoxelModelPoint3 translation{t.x, t.y, t.z};
        const bool hasTranslation = (t.x != 0) || (t.y != 0) || (t.z != 0);
        frames.push_back(pomdogschemas::CreateVoxelModelTransformFrame(
            builder,
            frame.frame,
            frame.rotation,
            hasTranslation ? &translation : nullptr,
            attributes));
    }

    std::vector<flatbuffers::Offset<pomdogschemas::VoxelModelShapeModel>> models;
    models.reserve(node.models.size());
    for (const auto& model : node.models) {
        const auto attributes = createAttributes(builder, model.attributes);
        models.push_back(pomdogschemas::CreateVoxelModelShapeModel(builder, model.grid, model.frame, attributes));
    }

    const auto name = createString(builder, node.name);
    const auto attributes = createAttributes(builder, node.attributes);
    const auto children = createVector(builder, node.children);
    const auto framesOffset = createVector(builder, frames);
    const auto modelsOffset = createVector(builder, models);
    return pomdogschemas::CreateVoxelModelNode(
        builder,
        static_cast<pomdogschemas::VoxelModelNodeType>(node.type),
        name,
        node.hidden,
        attributes,
        children,
        node.layer,
        framesOffset,
        modelsOffset);
}

void buildModel(flatbuffers::FlatBufferBuilder& builder, const VoxelModel& model)
{
    std::vector<pomdogschemas::VoxelModelColor> palette;
    palette.reserve(model.palette.size());
    for (const auto& c : model.palette) {
        palette.emplace_back(c.r, c.g, c.b, c.a);
    }
    const auto paletteOffset = builder.CreateVectorOfStructs(palette);

    std::vector<flatbuffers::Offset<pomdogschemas::VoxelModelGrid>> grids;
    grids.reserve(model.grids.size());
    for (const auto& grid : model.grids) {
        std::vector<pomdogschemas::VoxelModelVoxel> voxels;
        voxels.reserve(grid.voxels.size());
        for (const auto& v : grid.voxels) {
            voxels.emplace_back(v.x, v.y, v.z, v.colorIndex);
        }
        const auto voxelsOffset = voxels.empty() ? 0 : builder.CreateVectorOfStructs(voxels);
        grids.push_back(pomdogschemas::CreateVoxelModelGrid(builder, grid.sizeX, grid.sizeY, grid.sizeZ, voxelsOffset));
    }

    std::vector<flatbuffers::Offset<pomdogschemas::VoxelModelNode>> nodes;
    nodes.reserve(model.nodes.size());
    for (const auto& node : model.nodes) {
        nodes.push_back(createNode(builder, node));
    }

    std::vector<flatbuffers::Offset<pomdogschemas::VoxelModelLayer>> layers;
    layers.reserve(model.layers.size());
    for (const auto& layer : model.layers) {
        const auto name = createString(builder, layer.name);
        const auto attributes = createAttributes(builder, layer.attributes);
        layers.push_back(pomdogschemas::CreateVoxelModelLayer(builder, layer.id, name, layer.hidden, attributes));
    }

    std::vector<flatbuffers::Offset<pomdogschemas::VoxelModelMaterial>> materials;
    materials.reserve(model.materials.size());
    for (const auto& material : model.materials) {
        const auto properties = createAttributes(builder, material.properties);
        materials.push_back(pomdogschemas::CreateVoxelModelMaterial(builder, material.id, properties));
    }

    const auto notes = model.notes.empty() ? 0 : builder.CreateVectorOfStrings(model.notes);

    const auto gridsOffset = createVector(builder, grids);
    const auto nodesOffset = createVector(builder, nodes);
    const auto layersOffset = createVector(builder, layers);
    const auto materialsOffset = createVector(builder, materials);
    builder.Finish(pomdogschemas::CreateVoxelModel(
        builder,
        paletteOffset,
        gridsOffset,
        nodesOffset,
        layersOffset,
        materialsOffset,
        notes));
}

} // namespace

std::unique_ptr<Error>
saveVoxelModel(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    std::string_view filePath,
    const VoxelModel& model) noexcept
{
    const auto path = std::string{filePath};

    if (auto err = validateVoxelModel(model); err != nullptr) {
        return errors::wrap(std::move(err), "cannot save invalid voxel model, " + path);
    }

    flatbuffers::FlatBufferBuilder builder;
    buildModel(builder, model);
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    const auto bytes = std::span<const u8>{builder.GetBufferPointer(), builder.GetSize()};
    POMDOG_CLANG_UNSAFE_BUFFER_END

    auto [file, openErr] = vfs::open(fs, path, vfs::FileMode{.write = true, .create = true, .truncate = true});
    if (openErr != nullptr) {
        return errors::wrap(std::move(openErr), "cannot open voxel model file for writing, " + path);
    }

    auto [written, writeErr] = file->write(bytes);
    if (writeErr != nullptr) {
        return errors::wrap(std::move(writeErr), "cannot write voxel model file, " + path);
    }
    if (written != bytes.size()) {
        return errors::make(pomdog::format("wrote {} of {} bytes to voxel model file, {}", written, bytes.size(), path));
    }

    if (auto err = file->close(); err != nullptr) {
        return errors::wrap(std::move(err), "cannot close voxel model file, " + path);
    }
    return nullptr;
}

} // namespace pomdog::voxelmodel
