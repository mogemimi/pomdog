// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/voxelmodel/voxel_model_loader.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/voxel_model_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::voxelmodel {
namespace {

using AttributeVector = flatbuffers::Vector<flatbuffers::Offset<pomdogschemas::VoxelModelAttribute>>;

// NOTE: The node types of the file have the values of NodeType.
static_assert(static_cast<u8>(NodeType::Transform) == pomdogschemas::VoxelModelNodeType_Transform);
static_assert(static_cast<u8>(NodeType::Group) == pomdogschemas::VoxelModelNodeType_Group);
static_assert(static_cast<u8>(NodeType::Shape) == pomdogschemas::VoxelModelNodeType_Shape);

[[nodiscard]] std::string
toString(const flatbuffers::String* s)
{
    if (s == nullptr) {
        return {};
    }
    return s->str();
}

[[nodiscard]] std::vector<Attribute>
toAttributes(const AttributeVector* attributes)
{
    std::vector<Attribute> result;
    if (attributes == nullptr) {
        return result;
    }
    result.reserve(attributes->size());
    for (const auto* attribute : *attributes) {
        result.push_back(Attribute{
            .key = toString(attribute->key()),
            .value = toString(attribute->value()),
        });
    }
    return result;
}

[[nodiscard]] std::unique_ptr<Error>
readNode(const pomdogschemas::VoxelModelNode& source, Node& node)
{
    if (source.type() > pomdogschemas::VoxelModelNodeType_MAX) {
        return errors::make(pomdog::format("unknown node type {}", static_cast<u32>(source.type())));
    }
    node.type = static_cast<NodeType>(source.type());
    node.name = toString(source.name());
    node.hidden = source.hidden();
    node.attributes = toAttributes(source.attributes());
    if (const auto* children = source.children(); children != nullptr) {
        node.children.assign(children->begin(), children->end());
    }
    node.layer = source.layer();
    if (const auto* frames = source.frames(); frames != nullptr) {
        node.frames.reserve(frames->size());
        for (const auto* frame : *frames) {
            TransformFrame result;
            result.frame = frame->frame();
            result.rotation = frame->rotation();
            if (const auto* t = frame->translation(); t != nullptr) {
                result.translation = Point3D{t->x(), t->y(), t->z()};
            }
            result.attributes = toAttributes(frame->attributes());
            node.frames.push_back(std::move(result));
        }
    }
    if (const auto* models = source.models(); models != nullptr) {
        node.models.reserve(models->size());
        for (const auto* model : *models) {
            node.models.push_back(ShapeModel{
                .grid = model->grid(),
                .frame = model->frame(),
                .attributes = toAttributes(model->attributes()),
            });
        }
    }
    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
readModel(const pomdogschemas::VoxelModel& table, VoxelModel& model)
{
    const auto* palette = table.palette();
    if ((palette == nullptr) || (palette->size() != PaletteSize)) {
        return errors::make(pomdog::format("the palette has {} colors, not {}", (palette != nullptr) ? palette->size() : 0u, PaletteSize));
    }
    for (u32 i = 0; i < PaletteSize; ++i) {
        const auto* c = palette->Get(i);
        model.palette[i] = Color{c->r(), c->g(), c->b(), c->a()};
    }

    if (const auto* grids = table.grids(); grids != nullptr) {
        model.grids.reserve(grids->size());
        for (const auto* source : *grids) {
            VoxelGrid grid;
            grid.sizeX = source->size_x();
            grid.sizeY = source->size_y();
            grid.sizeZ = source->size_z();
            if (const auto* voxels = source->voxels(); voxels != nullptr) {
                grid.voxels.reserve(voxels->size());
                for (const auto* v : *voxels) {
                    grid.voxels.push_back(Voxel{.x = v->x(), .y = v->y(), .z = v->z(), .colorIndex = v->color_index()});
                }
            }
            model.grids.push_back(std::move(grid));
        }
    }

    if (const auto* nodes = table.nodes(); nodes != nullptr) {
        model.nodes.resize(nodes->size());
        for (u32 i = 0; i < nodes->size(); ++i) {
            if (auto err = readNode(*nodes->Get(i), model.nodes[i]); err != nullptr) {
                return errors::wrap(std::move(err), pomdog::format("node {}", i));
            }
        }
    }

    if (const auto* layers = table.layers(); layers != nullptr) {
        model.layers.reserve(layers->size());
        for (const auto* layer : *layers) {
            model.layers.push_back(Layer{
                .id = layer->id(),
                .name = toString(layer->name()),
                .hidden = layer->hidden(),
                .attributes = toAttributes(layer->attributes()),
            });
        }
    }

    if (const auto* materials = table.materials(); materials != nullptr) {
        model.materials.reserve(materials->size());
        for (const auto* material : *materials) {
            model.materials.push_back(Material{
                .id = material->id(),
                .properties = toAttributes(material->properties()),
            });
        }
    }

    if (const auto* notes = table.notes(); notes != nullptr) {
        model.notes.reserve(notes->size());
        for (const auto* note : *notes) {
            model.notes.push_back(toString(note));
        }
    }
    return nullptr;
}

} // namespace

std::tuple<VoxelModel, std::unique_ptr<Error>>
loadVoxelModel(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    std::string_view filePath) noexcept
{
    const auto path = std::string{filePath};

    auto [file, openErr] = vfs::open(fs, path);
    if (openErr != nullptr) {
        return {VoxelModel{}, errors::wrap(std::move(openErr), "cannot open voxel model file, " + path)};
    }

    auto [bytes, viewErr] = file->view();
    if (viewErr != nullptr) {
        return {VoxelModel{}, errors::wrap(std::move(viewErr), "cannot read voxel model file, " + path)};
    }

    flatbuffers::Verifier verifier(bytes.data(), bytes.size());
    if (!pomdogschemas::VerifyVoxelModelBuffer(verifier)) {
        return {VoxelModel{}, errors::make("FlatBuffers verification failed for voxel model file, " + path)};
    }

    VoxelModel model;
    if (auto err = readModel(*pomdogschemas::GetVoxelModel(bytes.data()), model); err != nullptr) {
        return {VoxelModel{}, errors::wrap(std::move(err), "invalid voxel model file, " + path)};
    }
    if (auto err = validateVoxelModel(model); err != nullptr) {
        return {VoxelModel{}, errors::wrap(std::move(err), "invalid voxel model file, " + path)};
    }
    return {std::move(model), nullptr};
}

} // namespace pomdog::voxelmodel
