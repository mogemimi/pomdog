// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/color.h"
#include "pomdog/math/point3d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <memory>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::voxelmodel {

/// The number of colors in a palette.
inline constexpr u32 PaletteSize = 256;

/// The largest size of a grid along an axis.
inline constexpr u32 MaxGridSize = 256;

/// The rotation that changes nothing, packed the way MagicaVoxel does.
inline constexpr u8 IdentityRotation = 4;

/// Voxel is a voxel of a grid: its position and the index of its color in
/// the palette.
struct Voxel final {
    u8 x = 0;
    u8 y = 0;
    u8 z = 0;

    /// The index of the color in VoxelModel::palette, between 1 and 255.
    u8 colorIndex = 0;
};

/// Attribute is an entry of a MagicaVoxel dictionary, such as "_rough" and
/// "0.1". Both the key and the value are text, as in the `.vox` file.
struct Attribute final {
    std::string key;
    std::string value;
};

/// VoxelGrid is a grid of voxels, which MagicaVoxel calls a model. The z axis
/// points up.
struct VoxelGrid final {
    /// The size of the grid along each axis, between 1 and MaxGridSize.
    u32 sizeX = 0;
    u32 sizeY = 0;
    u32 sizeZ = 0;

    /// The voxels inside the grid, no two of which share a position.
    std::vector<Voxel> voxels;
};

/// NodeType is the kind of a node of the scene graph.
enum class NodeType : u8 {
    /// Places its only child, which is a group or a shape.
    Transform = 0,

    /// Collects transforms.
    Group = 1,

    /// Shows grids and has no children.
    Shape = 2,
};

/// TransformFrame is a keyframe of a transform.
struct TransformFrame final {
    /// The frame number of the keyframe, which is 0 without animation.
    i32 frame = 0;

    /// A rotation matrix packed into a byte the way MagicaVoxel does: bits 0-1
    /// and 2-3 give the column of the nonzero entry of the first and the second
    /// row, and bits 4, 5, and 6 are set when the entry of the first, the
    /// second, and the third row is -1.
    u8 rotation = IdentityRotation;

    /// Moves the center of the child in the space of the parent, in voxels.
    Point3D translation = {0, 0, 0};

    /// The other entries of the frame dictionary.
    std::vector<Attribute> attributes;
};

/// ShapeModel is a grid that a shape shows from a frame on.
struct ShapeModel final {
    /// The index of the grid in VoxelModel::grids.
    u32 grid = 0;

    /// The frame number from which the shape shows the grid.
    i32 frame = 0;

    /// The other entries of the model dictionary.
    std::vector<Attribute> attributes;
};

/// Node is a node of the scene graph. The fields after `children` belong to
/// one type of node and stay empty for the others.
struct Node final {
    NodeType type = NodeType::Transform;

    /// The "_name" entry of the node dictionary.
    std::string name;

    /// The "_hidden" entry of the node dictionary.
    bool hidden = false;

    /// The other entries of the node dictionary.
    std::vector<Attribute> attributes;

    /// Indices of nodes in VoxelModel::nodes. A transform has exactly one
    /// child, which is a group or a shape; the children of a group are
    /// transforms; a shape has no children.
    std::vector<u32> children;

    /// The ID of the layer of a transform, or -1.
    i32 layer = -1;

    /// The keyframes of a transform in ascending order of their frames.
    std::vector<TransformFrame> frames;

    /// The grids of a shape in ascending order of their frames.
    std::vector<ShapeModel> models;
};

/// Layer is a layer of the scene.
struct Layer final {
    /// The ID that transforms refer to.
    i32 id = 0;

    /// The "_name" entry of the layer dictionary.
    std::string name;

    /// The "_hidden" entry of the layer dictionary.
    bool hidden = false;

    /// The other entries of the layer dictionary, such as "_color".
    std::vector<Attribute> attributes;
};

/// Material is a material of MagicaVoxel.
struct Material final {
    /// The palette index that the material belongs to.
    i32 id = 0;

    /// The entries of the material dictionary, such as "_type" and "_rough".
    std::vector<Attribute> properties;
};

/// VoxelModel is voxel data that converts to and from MagicaVoxel, which a
/// `.voxelmodel` file stores. `vox-to-voxelmodel` writes the file from a
/// `.vox` file, and `voxelmodel-to-vox` writes a `.vox` file back.
struct VoxelModel final {
    /// The colors of the voxels: `palette[i]` is the color of the voxels whose
    /// color index is `i`. No voxel uses `palette[0]`.
    std::array<Color, PaletteSize> palette;

    std::vector<VoxelGrid> grids;

    /// Either empty or a scene graph whose root is `nodes[0]`, a transform.
    /// Without a scene graph, the grids stand at the origin.
    std::vector<Node> nodes;

    std::vector<Layer> layers;

    std::vector<Material> materials;

    /// Describes the source data that a converter did not keep.
    std::vector<std::string> notes;

    /// Creates a model without grids, whose palette is transparent black.
    VoxelModel() noexcept;
};

/// Checks the rules that the fields of VoxelModel and its parts describe:
/// sizes of grids, voxels inside their grids without overlaps or color index
/// 0, a scene graph that forms a tree from a root transform, unique IDs of
/// layers and materials, and attributes without empty keys or keys that have
/// their own fields.
[[nodiscard]] POMDOG_EXPORT std::unique_ptr<Error>
validateVoxelModel(const VoxelModel& model) noexcept;

} // namespace pomdog::voxelmodel
