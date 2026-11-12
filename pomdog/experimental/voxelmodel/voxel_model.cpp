// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/voxelmodel/voxel_model.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <initializer_list>
#include <string_view>
#include <unordered_set>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::voxelmodel {
namespace {

constexpr std::string_view keyName = "_name";
constexpr std::string_view keyHidden = "_hidden";
constexpr std::string_view keyRotation = "_r";
constexpr std::string_view keyTranslation = "_t";
constexpr std::string_view keyFrame = "_f";

/// Checks that no attribute has an empty key or one of the reserved keys,
/// which have their own fields.
[[nodiscard]] std::unique_ptr<Error>
validateAttributes(
    const std::vector<Attribute>& attributes,
    std::initializer_list<std::string_view> reservedKeys) noexcept
{
    for (const auto& attribute : attributes) {
        if (attribute.key.empty()) {
            return errors::make("an attribute has an empty key");
        }
        for (const auto key : reservedKeys) {
            if (attribute.key == key) {
                return errors::make(pomdog::format("attribute \"{}\" belongs in its own field", attribute.key));
            }
        }
    }
    return nullptr;
}

/// Returns true when the byte packs a matrix whose rows have their nonzero
/// entries in different columns.
[[nodiscard]] bool
isValidRotation(u8 rotation) noexcept
{
    const auto c0 = static_cast<u32>(rotation & 3u);
    const auto c1 = static_cast<u32>((rotation >> 2) & 3u);
    return ((rotation & 0x80u) == 0) && (c0 < 3) && (c1 < 3) && (c0 != c1);
}

[[nodiscard]] std::unique_ptr<Error>
validateGrid(const VoxelGrid& grid) noexcept
{
    for (const auto size : {grid.sizeX, grid.sizeY, grid.sizeZ}) {
        if ((size < 1) || (size > MaxGridSize)) {
            return errors::make(pomdog::format("the size {}x{}x{} is not between 1 and {}", grid.sizeX, grid.sizeY, grid.sizeZ, MaxGridSize));
        }
    }

    std::vector<bool> occupied(static_cast<std::size_t>(grid.sizeX) * grid.sizeY * grid.sizeZ, false);
    for (const auto& voxel : grid.voxels) {
        if ((voxel.x >= grid.sizeX) || (voxel.y >= grid.sizeY) || (voxel.z >= grid.sizeZ)) {
            return errors::make(pomdog::format("the voxel at ({}, {}, {}) is outside the grid", voxel.x, voxel.y, voxel.z));
        }
        if (voxel.colorIndex == 0) {
            return errors::make(pomdog::format("the voxel at ({}, {}, {}) has the color index 0", voxel.x, voxel.y, voxel.z));
        }
        const auto cell = (static_cast<std::size_t>(voxel.z) * grid.sizeY + voxel.y) * grid.sizeX + voxel.x;
        if (occupied[cell]) {
            return errors::make(pomdog::format("two voxels at ({}, {}, {})", voxel.x, voxel.y, voxel.z));
        }
        occupied[cell] = true;
    }
    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
validateNode(const VoxelModel& model, const Node& node) noexcept
{
    if (auto err = validateAttributes(node.attributes, {keyName, keyHidden}); err != nullptr) {
        return err;
    }
    if (static_cast<u8>(node.type) > static_cast<u8>(NodeType::Shape)) {
        return errors::make(pomdog::format("unknown node type {}", static_cast<u32>(node.type)));
    }

    switch (node.type) {
    case NodeType::Transform:
        if (node.children.size() != 1) {
            return errors::make(pomdog::format("a transform has {} children, not 1", node.children.size()));
        }
        if (node.frames.empty()) {
            return errors::make("a transform has no frames");
        }
        for (std::size_t i = 0; i < node.frames.size(); ++i) {
            const auto& frame = node.frames[i];
            if ((frame.frame < 0) || ((i > 0) && (frame.frame <= node.frames[i - 1].frame))) {
                return errors::make("the frames are not in ascending order from 0");
            }
            if (!isValidRotation(frame.rotation)) {
                return errors::make(pomdog::format("frame {} has an invalid rotation {}", frame.frame, frame.rotation));
            }
            if (auto err = validateAttributes(frame.attributes, {keyRotation, keyTranslation, keyFrame}); err != nullptr) {
                return errors::wrap(std::move(err), pomdog::format("frame {}", frame.frame));
            }
        }
        if (!node.models.empty()) {
            return errors::make("a transform has models");
        }
        break;
    case NodeType::Group:
        if (!node.frames.empty() || !node.models.empty() || (node.layer != -1)) {
            return errors::make("a group has frames, models, or a layer");
        }
        break;
    case NodeType::Shape:
        if (!node.children.empty()) {
            return errors::make("a shape has children");
        }
        if (node.models.empty()) {
            return errors::make("a shape has no models");
        }
        for (std::size_t i = 0; i < node.models.size(); ++i) {
            const auto& shapeModel = node.models[i];
            if (shapeModel.grid >= model.grids.size()) {
                return errors::make(pomdog::format("model {} refers to grid {}, which is out of range", i, shapeModel.grid));
            }
            if ((shapeModel.frame < 0) || ((i > 0) && (shapeModel.frame <= node.models[i - 1].frame))) {
                return errors::make("the models are not in ascending order of frames from 0");
            }
            if (auto err = validateAttributes(shapeModel.attributes, {keyFrame}); err != nullptr) {
                return errors::wrap(std::move(err), pomdog::format("model {}", i));
            }
        }
        if (!node.frames.empty() || (node.layer != -1)) {
            return errors::make("a shape has frames or a layer");
        }
        break;
    }
    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
validateNodes(const VoxelModel& model) noexcept
{
    const auto& nodes = model.nodes;
    if (nodes.empty()) {
        return nullptr;
    }
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        if (auto err = validateNode(model, nodes[i]); err != nullptr) {
            return errors::wrap(std::move(err), pomdog::format("node {}", i));
        }
    }
    if (nodes.front().type != NodeType::Transform) {
        return errors::make("the root node is not a transform");
    }

    constexpr auto noParent = static_cast<std::size_t>(-1);
    std::vector<std::size_t> parents(nodes.size(), noParent);
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        const auto& node = nodes[i];
        for (const auto child : node.children) {
            if (child >= nodes.size()) {
                return errors::make(pomdog::format("node {}: child {} is out of range", i, child));
            }
            if (child == 0) {
                return errors::make(pomdog::format("node {}: the root node cannot be a child", i));
            }
            if (parents[child] != noParent) {
                return errors::make(pomdog::format("node {} has two parents, {} and {}", child, parents[child], i));
            }
            parents[child] = i;

            const auto childType = nodes[child].type;
            if ((node.type == NodeType::Transform) && (childType == NodeType::Transform)) {
                return errors::make(pomdog::format("node {}: the child of a transform must be a group or a shape", i));
            }
            if ((node.type == NodeType::Group) && (childType != NodeType::Transform)) {
                return errors::make(pomdog::format("node {}: the children of a group must be transforms", i));
            }
        }
    }

    // NOTE: Every node but the root has one parent, so the nodes form a tree
    // when the root reaches all of them.
    std::size_t reached = 0;
    std::vector<u32> stack = {0};
    while (!stack.empty()) {
        const auto index = stack.back();
        stack.pop_back();
        ++reached;
        const auto& children = nodes[index].children;
        stack.insert(stack.end(), children.begin(), children.end());
    }
    if (reached != nodes.size()) {
        return errors::make(pomdog::format("{} nodes are not reachable from the root node", nodes.size() - reached));
    }
    return nullptr;
}

} // namespace

VoxelModel::VoxelModel() noexcept
{
    palette.fill(Color{0, 0, 0, 0});
}

std::unique_ptr<Error>
validateVoxelModel(const VoxelModel& model) noexcept
{
    for (std::size_t i = 0; i < model.grids.size(); ++i) {
        if (auto err = validateGrid(model.grids[i]); err != nullptr) {
            return errors::wrap(std::move(err), pomdog::format("grid {}", i));
        }
    }

    if (auto err = validateNodes(model); err != nullptr) {
        return err;
    }

    std::unordered_set<i32> layerIDs;
    for (const auto& layer : model.layers) {
        if (!layerIDs.insert(layer.id).second) {
            return errors::make(pomdog::format("two layers have the ID {}", layer.id));
        }
        if (auto err = validateAttributes(layer.attributes, {keyName, keyHidden}); err != nullptr) {
            return errors::wrap(std::move(err), pomdog::format("layer {}", layer.id));
        }
    }

    std::unordered_set<i32> materialIDs;
    for (const auto& material : model.materials) {
        if (!materialIDs.insert(material.id).second) {
            return errors::make(pomdog::format("two materials have the ID {}", material.id));
        }
        if (auto err = validateAttributes(material.properties, {}); err != nullptr) {
            return errors::wrap(std::move(err), pomdog::format("material {}", material.id));
        }
    }
    return nullptr;
}

} // namespace pomdog::voxelmodel
