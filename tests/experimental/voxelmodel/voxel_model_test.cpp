// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/voxelmodel/voxel_model.h"
#include "pomdog/experimental/voxelmodel/voxel_model_loader.h"
#include "pomdog/experimental/voxelmodel/voxel_model_writer.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/utility/errors.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/voxel_model_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::Color;
using pomdog::Error;
using pomdog::Point3D;
using pomdog::u32;
using pomdog::u8;
using pomdog::voxelmodel::Attribute;
using pomdog::voxelmodel::Layer;
using pomdog::voxelmodel::Material;
using pomdog::voxelmodel::Node;
using pomdog::voxelmodel::NodeType;
using pomdog::voxelmodel::ShapeModel;
using pomdog::voxelmodel::TransformFrame;
using pomdog::voxelmodel::Voxel;
using pomdog::voxelmodel::VoxelGrid;
using pomdog::voxelmodel::VoxelModel;

namespace {

/// TemporaryDirectory makes a directory in the temporary directory, mounts it
/// at "/tmp" with write access, and removes it when it goes out of scope.
class TemporaryDirectory final {
private:
    std::filesystem::path path_;
    std::shared_ptr<pomdog::vfs::FileSystemContext> fs_;

public:
    TemporaryDirectory()
    {
        // NOTE: Every directory gets its own name, so that a file that a test
        // keeps open does not get in the way of the next test.
        static int serial = 0;
        const auto name = "pomdog_voxel_model_test_" + std::to_string(serial);
        ++serial;

        auto [tempDir, tempErr] = pomdog::platformfs::getTempDirectoryPath();
        REQUIRE(tempErr == nullptr);
        path_ = std::filesystem::path{tempDir} / name;
        std::error_code err;
        std::filesystem::create_directories(path_, err);
        REQUIRE(!err);

        auto [ctx, ctxErr] = pomdog::vfs::create();
        REQUIRE(ctxErr == nullptr);
        auto mountErr = pomdog::vfs::mount(ctx, "/tmp", path_.string(), {});
        REQUIRE(mountErr == nullptr);
        fs_ = std::move(ctx);
    }

    TemporaryDirectory(const TemporaryDirectory&) = delete;
    TemporaryDirectory& operator=(const TemporaryDirectory&) = delete;
    TemporaryDirectory(TemporaryDirectory&&) = delete;
    TemporaryDirectory& operator=(TemporaryDirectory&&) = delete;

    ~TemporaryDirectory()
    {
        fs_.reset();
        std::error_code err;
        std::filesystem::remove_all(path_, err);
    }

    [[nodiscard]] const std::shared_ptr<pomdog::vfs::FileSystemContext>&
    fs() const noexcept
    {
        return fs_;
    }

    /// Writes bytes to a file in the directory, and returns its virtual path.
    [[nodiscard]] std::string
    write(const std::string& name, std::span<const u8> bytes) const
    {
        std::ofstream stream(path_ / name, std::ios::binary | std::ios::trunc);
        REQUIRE(stream.is_open());
        stream.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
        REQUIRE(stream.good());
        return "/tmp/" + name;
    }

    [[nodiscard]] bool
    exists(const std::string& name) const
    {
        return std::filesystem::exists(path_ / name);
    }
};

[[nodiscard]] std::vector<u8>
toBytes(const flatbuffers::FlatBufferBuilder& builder)
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    const auto bytes = std::span<const u8>{builder.GetBufferPointer(), builder.GetSize()};
    POMDOG_CLANG_UNSAFE_BUFFER_END
    return std::vector<u8>(bytes.begin(), bytes.end());
}

[[nodiscard]] std::vector<u8>
pack(const pomdogschemas::VoxelModelT& model)
{
    flatbuffers::FlatBufferBuilder builder;
    builder.Finish(pomdogschemas::VoxelModel::Pack(builder, &model));
    return toBytes(builder);
}

/// Returns a valid table of a voxel model with a palette and one grid.
[[nodiscard]] pomdogschemas::VoxelModelT
makeTable()
{
    pomdogschemas::VoxelModelT model;
    for (u32 i = 0; i < 256; ++i) {
        model.palette.emplace_back(static_cast<u8>(i), u8(0), u8(0), u8(255));
    }
    auto grid = std::make_unique<pomdogschemas::VoxelModelGridT>();
    grid->size_x = 2;
    grid->size_y = 2;
    grid->size_z = 2;
    grid->voxels.emplace_back(u8(0), u8(0), u8(0), u8(1));
    model.grids.push_back(std::move(grid));
    return model;
}

/// Returns a model with two grids and a scene graph: the root transform, a
/// group, and two transforms with a shape each.
[[nodiscard]] VoxelModel
makeModel()
{
    VoxelModel model;
    for (u32 i = 0; i < pomdog::voxelmodel::PaletteSize; ++i) {
        model.palette[i] = Color{static_cast<u8>(i), static_cast<u8>(255 - i), 7, 255};
    }

    VoxelGrid grid0;
    grid0.sizeX = 3;
    grid0.sizeY = 2;
    grid0.sizeZ = 1;
    grid0.voxels.push_back(Voxel{.x = 0, .y = 0, .z = 0, .colorIndex = 1});
    grid0.voxels.push_back(Voxel{.x = 2, .y = 1, .z = 0, .colorIndex = 255});
    model.grids.push_back(std::move(grid0));

    VoxelGrid grid1;
    grid1.sizeX = 256;
    grid1.sizeY = 1;
    grid1.sizeZ = 1;
    grid1.voxels.push_back(Voxel{.x = 255, .y = 0, .z = 0, .colorIndex = 9});
    model.grids.push_back(std::move(grid1));

    Node root;
    root.type = NodeType::Transform;
    root.children = {1};
    root.frames.push_back(TransformFrame{});
    model.nodes.push_back(std::move(root));

    Node group;
    group.type = NodeType::Group;
    group.children = {2, 4};
    group.attributes.push_back(Attribute{.key = "_g", .value = "1"});
    model.nodes.push_back(std::move(group));

    Node arm;
    arm.type = NodeType::Transform;
    arm.name = "arm";
    arm.hidden = true;
    arm.children = {3};
    arm.layer = 2;
    arm.frames.push_back(TransformFrame{.frame = 0, .rotation = 17, .translation = Point3D{1, -2, 3}, .attributes = {}});
    arm.frames.push_back(TransformFrame{.frame = 5, .rotation = 4, .translation = Point3D{0, 0, 0}, .attributes = {Attribute{.key = "_x", .value = "y"}}});
    model.nodes.push_back(std::move(arm));

    Node armShape;
    armShape.type = NodeType::Shape;
    armShape.models.push_back(ShapeModel{.grid = 0, .frame = 0, .attributes = {}});
    armShape.models.push_back(ShapeModel{.grid = 1, .frame = 5, .attributes = {Attribute{.key = "_m", .value = "n"}}});
    model.nodes.push_back(std::move(armShape));

    Node leg;
    leg.type = NodeType::Transform;
    leg.children = {5};
    leg.layer = 0;
    leg.frames.push_back(TransformFrame{});
    model.nodes.push_back(std::move(leg));

    Node legShape;
    legShape.type = NodeType::Shape;
    legShape.models.push_back(ShapeModel{});
    model.nodes.push_back(std::move(legShape));

    model.layers.push_back(Layer{.id = 2, .name = "top", .hidden = false, .attributes = {Attribute{.key = "_color", .value = "1 2 3"}}});
    model.layers.push_back(Layer{.id = 0, .name = "", .hidden = true, .attributes = {}});
    model.materials.push_back(Material{.id = 9, .properties = {Attribute{.key = "_type", .value = "_metal"}, Attribute{.key = "_rough", .value = "0.1"}}});
    model.notes.push_back("dropped 1 \"rCAM\" chunks: cameras");
    return model;
}

void checkAttributes(const std::vector<Attribute>& actual, const std::vector<Attribute>& expected)
{
    REQUIRE(actual.size() == expected.size());
    for (std::size_t i = 0; i < actual.size(); ++i) {
        CHECK(actual[i].key == expected[i].key);
        CHECK(actual[i].value == expected[i].value);
    }
}

void checkSameModel(const VoxelModel& actual, const VoxelModel& expected)
{
    for (std::size_t i = 0; i < expected.palette.size(); ++i) {
        CHECK(actual.palette[i] == expected.palette[i]);
    }

    REQUIRE(actual.grids.size() == expected.grids.size());
    for (std::size_t i = 0; i < expected.grids.size(); ++i) {
        const auto& a = actual.grids[i];
        const auto& e = expected.grids[i];
        CHECK(a.sizeX == e.sizeX);
        CHECK(a.sizeY == e.sizeY);
        CHECK(a.sizeZ == e.sizeZ);
        REQUIRE(a.voxels.size() == e.voxels.size());
        for (std::size_t j = 0; j < e.voxels.size(); ++j) {
            CHECK(a.voxels[j].x == e.voxels[j].x);
            CHECK(a.voxels[j].y == e.voxels[j].y);
            CHECK(a.voxels[j].z == e.voxels[j].z);
            CHECK(a.voxels[j].colorIndex == e.voxels[j].colorIndex);
        }
    }

    REQUIRE(actual.nodes.size() == expected.nodes.size());
    for (std::size_t i = 0; i < expected.nodes.size(); ++i) {
        const auto& a = actual.nodes[i];
        const auto& e = expected.nodes[i];
        CHECK(a.type == e.type);
        CHECK(a.name == e.name);
        CHECK(a.hidden == e.hidden);
        checkAttributes(a.attributes, e.attributes);
        CHECK(a.children == e.children);
        CHECK(a.layer == e.layer);
        REQUIRE(a.frames.size() == e.frames.size());
        for (std::size_t j = 0; j < e.frames.size(); ++j) {
            CHECK(a.frames[j].frame == e.frames[j].frame);
            CHECK(a.frames[j].rotation == e.frames[j].rotation);
            CHECK(a.frames[j].translation == e.frames[j].translation);
            checkAttributes(a.frames[j].attributes, e.frames[j].attributes);
        }
        REQUIRE(a.models.size() == e.models.size());
        for (std::size_t j = 0; j < e.models.size(); ++j) {
            CHECK(a.models[j].grid == e.models[j].grid);
            CHECK(a.models[j].frame == e.models[j].frame);
            checkAttributes(a.models[j].attributes, e.models[j].attributes);
        }
    }

    REQUIRE(actual.layers.size() == expected.layers.size());
    for (std::size_t i = 0; i < expected.layers.size(); ++i) {
        CHECK(actual.layers[i].id == expected.layers[i].id);
        CHECK(actual.layers[i].name == expected.layers[i].name);
        CHECK(actual.layers[i].hidden == expected.layers[i].hidden);
        checkAttributes(actual.layers[i].attributes, expected.layers[i].attributes);
    }

    REQUIRE(actual.materials.size() == expected.materials.size());
    for (std::size_t i = 0; i < expected.materials.size(); ++i) {
        CHECK(actual.materials[i].id == expected.materials[i].id);
        checkAttributes(actual.materials[i].properties, expected.materials[i].properties);
    }

    CHECK(actual.notes == expected.notes);
}

} // namespace

TEST_CASE("VoxelModel")
{
    SUBCASE("a default model has a transparent black palette and passes validation")
    {
        VoxelModel model;
        for (const auto& color : model.palette) {
            CHECK(color == Color{0, 0, 0, 0});
        }
        const auto err = pomdog::voxelmodel::validateVoxelModel(model);
        CHECK(err == nullptr);
    }
    SUBCASE("a model with a scene graph passes validation")
    {
        const auto err = pomdog::voxelmodel::validateVoxelModel(makeModel());
        CHECK(err == nullptr);
    }
}

TEST_CASE("validateVoxelModel")
{
    struct Case final {
        const char* name;
        std::function<void(VoxelModel&)> modify;
    };
    const std::vector<Case> cases = {
        {"grid size zero", [](VoxelModel& m) { m.grids[0].sizeY = 0; }},
        {"grid size large", [](VoxelModel& m) { m.grids[0].sizeZ = 257; }},
        {"voxel outside", [](VoxelModel& m) { m.grids[0].voxels[1].x = 3; }},
        {"color index 0", [](VoxelModel& m) { m.grids[0].voxels[0].colorIndex = 0; }},
        {"two voxels at one position", [](VoxelModel& m) { m.grids[0].voxels[1] = Voxel{.x = 0, .y = 0, .z = 0, .colorIndex = 2}; }},
        {"root is not a transform", [](VoxelModel& m) {
             m.nodes.erase(m.nodes.begin(), m.nodes.begin() + 3);
             m.nodes.resize(1);
         }},
        {"transform with two children", [](VoxelModel& m) { m.nodes[2].children = {3, 5}; }},
        {"transform of a transform", [](VoxelModel& m) { m.nodes[0].children = {2}; }},
        {"group of a shape", [](VoxelModel& m) { m.nodes[1].children = {2, 3}; }},
        {"child out of range", [](VoxelModel& m) { m.nodes[1].children = {2, 9}; }},
        {"root as a child", [](VoxelModel& m) { m.nodes[1].children = {2, 0}; }},
        {"two parents", [](VoxelModel& m) { m.nodes[4].children = {3}; }},
        {"unreachable node", [](VoxelModel& m) { m.nodes[1].children = {2}; }},
        {"transform without frames", [](VoxelModel& m) { m.nodes[0].frames.clear(); }},
        {"frames out of order", [](VoxelModel& m) { m.nodes[2].frames[1].frame = 0; }},
        {"negative frame", [](VoxelModel& m) { m.nodes[0].frames[0].frame = -1; }},
        {"invalid rotation", [](VoxelModel& m) { m.nodes[0].frames[0].rotation = 0; }},
        {"reserved frame key", [](VoxelModel& m) { m.nodes[0].frames[0].attributes.push_back(Attribute{.key = "_t", .value = "0 0 0"}); }},
        {"reserved node key", [](VoxelModel& m) { m.nodes[0].attributes.push_back(Attribute{.key = "_name", .value = "x"}); }},
        {"empty key", [](VoxelModel& m) { m.materials[0].properties.push_back(Attribute{.key = "", .value = "x"}); }},
        {"group with a layer", [](VoxelModel& m) { m.nodes[1].layer = 0; }},
        {"shape with a missing grid", [](VoxelModel& m) { m.nodes[3].models[0].grid = 2; }},
        {"shape without models", [](VoxelModel& m) { m.nodes[3].models.clear(); }},
        {"shape with children", [](VoxelModel& m) { m.nodes[3].children = {5}; }},
        {"unknown node type", [](VoxelModel& m) { m.nodes[5].type = static_cast<NodeType>(3); }},
        {"two layers with one ID", [](VoxelModel& m) { m.layers[1].id = 2; }},
        {"reserved layer key", [](VoxelModel& m) { m.layers[0].attributes.push_back(Attribute{.key = "_hidden", .value = "1"}); }},
        {"two materials with one ID", [](VoxelModel& m) { m.materials.push_back(Material{.id = 9, .properties = {}}); }},
    };
    for (const auto& c : cases) {
        CAPTURE(c.name);
        auto model = makeModel();
        c.modify(model);
        const auto err = pomdog::voxelmodel::validateVoxelModel(model);
        CHECK(err != nullptr);
    }
}

TEST_CASE("saveVoxelModel and loadVoxelModel")
{
    TemporaryDirectory dir;

    SUBCASE("a saved model loads back unchanged")
    {
        const auto model = makeModel();
        const auto saveErr = pomdog::voxelmodel::saveVoxelModel(dir.fs(), "/tmp/model.voxelmodel", model);
        REQUIRE(saveErr == nullptr);

        auto [loaded, err] = pomdog::voxelmodel::loadVoxelModel(dir.fs(), "/tmp/model.voxelmodel");
        REQUIRE(err == nullptr);
        checkSameModel(loaded, model);

        // NOTE: Saving again replaces the file.
        auto edited = std::move(loaded);
        edited.grids[0].voxels.pop_back();
        edited.palette[1] = Color{1, 2, 3, 4};
        const auto resaveErr = pomdog::voxelmodel::saveVoxelModel(dir.fs(), "/tmp/model.voxelmodel", edited);
        REQUIRE(resaveErr == nullptr);
        auto [reloaded, reloadErr] = pomdog::voxelmodel::loadVoxelModel(dir.fs(), "/tmp/model.voxelmodel");
        REQUIRE(reloadErr == nullptr);
        checkSameModel(reloaded, edited);
    }
    SUBCASE("an invalid model is not saved")
    {
        auto model = makeModel();
        model.grids[0].voxels[0].colorIndex = 0;
        const auto err = pomdog::voxelmodel::saveVoxelModel(dir.fs(), "/tmp/invalid.voxelmodel", model);
        CHECK(err != nullptr);
        CHECK(!dir.exists("invalid.voxelmodel"));
    }
    SUBCASE("a table that the tools write loads")
    {
        const auto path = dir.write("table.voxelmodel", pack(makeTable()));
        auto [model, err] = pomdog::voxelmodel::loadVoxelModel(dir.fs(), path);
        REQUIRE(err == nullptr);
        REQUIRE(model.grids.size() == 1);
        CHECK(model.grids[0].voxels.size() == 1);
        CHECK(model.palette[3] == Color{3, 0, 0, 255});
        CHECK(model.nodes.empty());
    }
    SUBCASE("a missing file is an error")
    {
        auto [model, err] = pomdog::voxelmodel::loadVoxelModel(dir.fs(), "/tmp/missing.voxelmodel");
        CHECK(err != nullptr);
    }
    SUBCASE("bytes that are not a voxel model are an error")
    {
        const std::vector<u8> bytes = {1, 2, 3, 4, 5, 6, 7, 8};
        const auto path = dir.write("garbage.voxelmodel", bytes);
        auto [model, err] = pomdog::voxelmodel::loadVoxelModel(dir.fs(), path);
        CHECK(err != nullptr);
    }
    SUBCASE("a palette without 256 colors is an error")
    {
        auto table = makeTable();
        table.palette.pop_back();
        const auto path = dir.write("palette.voxelmodel", pack(table));
        auto [model, err] = pomdog::voxelmodel::loadVoxelModel(dir.fs(), path);
        CHECK(err != nullptr);
    }
    SUBCASE("an unknown node type is an error")
    {
        auto table = makeTable();
        auto node = std::make_unique<pomdogschemas::VoxelModelNodeT>();
        node->type = static_cast<pomdogschemas::VoxelModelNodeType>(7);
        table.nodes.push_back(std::move(node));
        const auto path = dir.write("node.voxelmodel", pack(table));
        auto [model, err] = pomdog::voxelmodel::loadVoxelModel(dir.fs(), path);
        CHECK(err != nullptr);
    }
    SUBCASE("a file that breaks a rule is an error")
    {
        auto table = makeTable();
        table.grids[0]->voxels.emplace_back(u8(2), u8(0), u8(0), u8(1));
        const auto path = dir.write("rule.voxelmodel", pack(table));
        auto [model, err] = pomdog::voxelmodel::loadVoxelModel(dir.fs(), path);
        CHECK(err != nullptr);
    }
}
