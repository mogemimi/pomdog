// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"

// FIXME: Suppress warnings for the Animator and AnimationGraphState classes,
// whose copy operations are implicitly deleted.
POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(4625)
POMDOG_MSVC_SUPPRESS_WARNING(4626)
#include "pomdog/experimental/skeletal2d/animator.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_graph.h"
#include "pomdog/experimental/spine/animation_graph_builder.h"
POMDOG_MSVC_SUPPRESS_WARNING_POP

#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/basic/types.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/experimental/skeletal2d/animation_clip.h"
#include "pomdog/experimental/skeletal2d/skeleton.h"
#include "pomdog/experimental/skeletal2d/skeleton_helper.h"
#include "pomdog/experimental/skeletal2d/skeleton_pose.h"
#include "pomdog/experimental/skeletal2d/skin.h"
#include "pomdog/experimental/skeletal2d/skinned_mesh.h"
#include "pomdog/experimental/spine/animation_clip_data.h"
#include "pomdog/experimental/spine/animation_loader.h"
#include "pomdog/experimental/spine/skeleton_data.h"
#include "pomdog/experimental/spine/skeleton_loader.h"
#include "pomdog/experimental/spine/skin_data.h"
#include "pomdog/experimental/spine/skin_loader.h"
#include "pomdog/experimental/spine/skinned_mesh_loader.h"
#include "pomdog/experimental/texture_atlas/texture_atlas.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_hash32.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/skeletal2d_animgraph_generated.h"
#include "pomdogschemas/skeletal2d_clip_generated.h"
#include "pomdogschemas/skeletal2d_skeleton_generated.h"
#include "pomdogschemas/skeletal2d_skin_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
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
using pomdog::Duration;
using pomdog::Error;
using pomdog::f32;
using pomdog::Matrix3x2;
using pomdog::TextureRegion;
using pomdog::u16;
using pomdog::u32;
using pomdog::u8;
using pomdog::Vector2;
using pomdog::skeletal2d::AnimationGraph;
using pomdog::skeletal2d::AnimationTimeInterval;
using pomdog::skeletal2d::JointIndex;
using pomdog::spine::AnimationClipData;
using pomdog::spine::SkeletonData;
using pomdog::spine::SkinData;

namespace {

constexpr u8 noJoint = 255;
constexpr u16 noMesh = 65535;

/// TemporaryFile writes bytes to a file in the temporary directory, and removes
/// the file when it goes out of scope.
class TemporaryFile final {
private:
    std::filesystem::path path_;
    std::string directory_;
    std::string name_;

public:
    TemporaryFile(std::string_view extension, std::span<const u8> bytes)
    {
        // NOTE: A file stays open while its data is alive, so every file gets
        // its own name.
        static int serial = 0;
        name_ = "pomdog_spine_loader_test_" + std::to_string(serial) + std::string{extension};
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

/// Loaded holds data with the file that it reads. The data closes the file
/// before the file is removed, because members are destroyed in reverse order.
template <typename Data>
struct Loaded final {
    std::unique_ptr<TemporaryFile> file;
    std::shared_ptr<Data> data;
    std::unique_ptr<Error> err;

    Loaded() = default;
    Loaded(const Loaded&) = delete;
    Loaded& operator=(const Loaded&) = delete;
    Loaded(Loaded&&) = default;
    Loaded& operator=(Loaded&&) = default;
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
pack(const pomdogschemas::Skeletal2DSkeletonT& skeleton)
{
    flatbuffers::FlatBufferBuilder builder;
    pomdogschemas::FinishSkeletal2DSkeletonBuffer(builder, pomdogschemas::Skeletal2DSkeleton::Pack(builder, &skeleton));
    return toBytes(builder);
}

[[nodiscard]] std::vector<u8>
pack(const pomdogschemas::Skeletal2DSkinSetT& skinSet)
{
    flatbuffers::FlatBufferBuilder builder;
    pomdogschemas::FinishSkeletal2DSkinSetBuffer(builder, pomdogschemas::Skeletal2DSkinSet::Pack(builder, &skinSet));
    return toBytes(builder);
}

[[nodiscard]] std::vector<u8>
pack(const pomdogschemas::Skeletal2DClipSetT& clipSet)
{
    flatbuffers::FlatBufferBuilder builder;
    pomdogschemas::FinishSkeletal2DClipSetBuffer(builder, pomdogschemas::Skeletal2DClipSet::Pack(builder, &clipSet));
    return toBytes(builder);
}

[[nodiscard]] std::vector<u8>
pack(const pomdogschemas::Skeletal2DAnimGraphT& graph)
{
    flatbuffers::FlatBufferBuilder builder;
    pomdogschemas::FinishSkeletal2DAnimGraphBuffer(builder, pomdogschemas::Skeletal2DAnimGraph::Pack(builder, &graph));
    return toBytes(builder);
}

[[nodiscard]] Loaded<SkeletonData>
loadSkeletonBytes(std::span<const u8> bytes)
{
    Loaded<SkeletonData> result;
    result.file = std::make_unique<TemporaryFile>(".skeleton_skel2d", bytes);
    std::tie(result.data, result.err) = pomdog::spine::loadSkeletonData(result.file->mount(), result.file->getPath());
    return result;
}

[[nodiscard]] Loaded<SkeletonData>
load(const pomdogschemas::Skeletal2DSkeletonT& skeleton)
{
    return loadSkeletonBytes(pack(skeleton));
}

[[nodiscard]] Loaded<SkinData>
load(const pomdogschemas::Skeletal2DSkinSetT& skinSet)
{
    Loaded<SkinData> result;
    result.file = std::make_unique<TemporaryFile>(".skin_skel2d", pack(skinSet));
    std::tie(result.data, result.err) = pomdog::spine::loadSkinData(result.file->mount(), result.file->getPath());
    return result;
}

[[nodiscard]] Loaded<AnimationClipData>
load(const pomdogschemas::Skeletal2DClipSetT& clipSet)
{
    Loaded<AnimationClipData> result;
    result.file = std::make_unique<TemporaryFile>(".clip_skel2d", pack(clipSet));
    std::tie(result.data, result.err) = pomdog::spine::loadAnimationClipData(result.file->mount(), result.file->getPath());
    return result;
}

[[nodiscard]] Loaded<AnimationGraph>
loadGraphBytes(std::span<const u8> bytes, const AnimationClipData& clipData)
{
    Loaded<AnimationGraph> result;
    result.file = std::make_unique<TemporaryFile>(".animgraph_skel2d", bytes);
    std::tie(result.data, result.err) = pomdog::spine::loadAnimationGraph(result.file->mount(), clipData, result.file->getPath());
    return result;
}

[[nodiscard]] Loaded<AnimationGraph>
load(const pomdogschemas::Skeletal2DAnimGraphT& graph, const AnimationClipData& clipData)
{
    return loadGraphBytes(pack(graph), clipData);
}

/// Returns a skeleton of four joints:
/// ```
/// 0 (root)
/// ├── 1
/// │   └── 3
/// └── 2
/// ```
[[nodiscard]] pomdogschemas::Skeletal2DSkeletonT
makeSkeleton()
{
    pomdogschemas::Skeletal2DSkeletonT skeleton;
    skeleton.joints.emplace_back(0.0f, 0.0f, 0.0f, 1.0f, noJoint);
    skeleton.joints.emplace_back(10.0f, 20.0f, pomdog::math::PiOver2<f32>, 1.0f, u8(0));
    skeleton.joints.emplace_back(-5.0f, 0.0f, 0.0f, 2.0f, u8(0));
    skeleton.joints.emplace_back(3.0f, 4.0f, -0.5f, 1.0f, u8(1));
    return skeleton;
}

/// FakeTextureAtlas is an atlas of regions without a texture.
class FakeTextureAtlas final : public pomdog::TextureAtlas {
private:
    std::vector<u32> keys_;
    std::vector<TextureRegion> regions_;

public:
    void add(std::string_view name, const TextureRegion& region)
    {
        keys_.push_back(computeStringHash32(name));
        regions_.push_back(region);
    }

    [[nodiscard]] std::shared_ptr<pomdog::gpu::Texture2D>
    getTexture() const noexcept override
    {
        return nullptr;
    }

    [[nodiscard]] TextureRegion
    getRegion(u32 index) const noexcept override
    {
        return regions_[index];
    }

    [[nodiscard]] TextureRegion
    getRegionByKey(u32 key) const noexcept override
    {
        if (const auto index = findRegionByKey(key); index.has_value()) {
            return regions_[*index];
        }
        return TextureRegion{};
    }

    [[nodiscard]] std::optional<u32>
    findRegionByKey(u32 key) const noexcept override
    {
        const auto iter = std::find(keys_.begin(), keys_.end(), key);
        if (iter == keys_.end()) {
            return std::nullopt;
        }
        return static_cast<u32>(std::distance(keys_.begin(), iter));
    }

    [[nodiscard]] bool
    isPremultipliedAlpha() const noexcept override
    {
        return false;
    }
};

[[nodiscard]] TextureRegion
makeRegion(pomdog::i16 subrectX, pomdog::i16 subrectY)
{
    TextureRegion region;
    region.subrectX = subrectX;
    region.subrectY = subrectY;
    region.subrectWidth = 30;
    region.subrectHeight = 40;
    region.xOffset = 2;
    region.yOffset = 4;
    region.width = 36;
    region.height = 48;
    return region;
}

/// Returns a skin "default" whose slot "body" shows the region "image" on
/// joint 1, and whose slot "cloth" shows the mesh "cloth_mesh" on joint 2.
[[nodiscard]] pomdogschemas::Skeletal2DSkinSetT
makeSkinSet()
{
    auto mesh = std::make_unique<pomdogschemas::Skeletal2DMeshT>();
    mesh->vertices.emplace_back(1.0f, 2.0f, 0.0f, 0.0f, 0.75f, 0.25f, 0.0f, 0.0f, u8(2), u8(1), noJoint, noJoint);
    mesh->vertices.emplace_back(3.0f, 4.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, u8(2), noJoint, noJoint, noJoint);
    mesh->vertices.emplace_back(5.0f, 6.0f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, u8(3), noJoint, noJoint, noJoint);
    mesh->indices = {0, 1, 2};

    auto skin = std::make_unique<pomdogschemas::Skeletal2DSkinT>();
    skin->slots.emplace_back(computeStringHash32("body"), computeStringHash32("image"), 7.0f, 8.0f, 1.0f, 2.0f, 0.5f, noMesh, u8(1));
    skin->slots.emplace_back(computeStringHash32("cloth"), computeStringHash32("cloth_mesh"), 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, u16(0), u8(2));
    skin->meshes.push_back(std::move(mesh));

    pomdogschemas::Skeletal2DSkinSetT skinSet;
    skinSet.keys.push_back(computeStringHash32("default"));
    skinSet.skins.push_back(std::move(skin));
    return skinSet;
}

[[nodiscard]] std::shared_ptr<FakeTextureAtlas>
makeAtlas()
{
    auto atlas = std::make_shared<FakeTextureAtlas>();
    atlas->add("image", makeRegion(10, 20));
    atlas->add("blink", makeRegion(50, 60));
    atlas->add("cloth_mesh", makeRegion(100, 120));
    return atlas;
}

/// Returns a clip "walk" that rotates and translates joint 1, scales joint 2,
/// and switches the attachment of the slot "body".
[[nodiscard]] pomdogschemas::Skeletal2DClipSetT
makeClipSet()
{
    auto rotation = std::make_unique<pomdogschemas::Skeletal2DRotationTrackT>();
    rotation->joint = 1;
    rotation->keys.emplace_back(0.0f, 0.0f);
    rotation->keys.emplace_back(1.0f, 1.0f);

    auto scale = std::make_unique<pomdogschemas::Skeletal2DScaleTrackT>();
    scale->joint = 2;
    scale->keys.emplace_back(0.0f, 2.0f);

    auto translation = std::make_unique<pomdogschemas::Skeletal2DTranslationTrackT>();
    translation->joint = 1;
    translation->keys.emplace_back(0.0f, 3.0f, 4.0f);

    auto attachment = std::make_unique<pomdogschemas::Skeletal2DAttachmentTrackT>();
    attachment->slot_key = computeStringHash32("body");
    attachment->keys.emplace_back(0.0f, computeStringHash32("image"));
    attachment->keys.emplace_back(0.5f, computeStringHash32("blink"));

    auto clip = std::make_unique<pomdogschemas::Skeletal2DClipT>();
    clip->rotation_tracks.push_back(std::move(rotation));
    clip->scale_tracks.push_back(std::move(scale));
    clip->translation_tracks.push_back(std::move(translation));
    clip->attachment_tracks.push_back(std::move(attachment));

    // NOTE: The keys must be in ascending order.
    std::vector<std::pair<u32, std::unique_ptr<pomdogschemas::Skeletal2DClipT>>> clips;
    clips.emplace_back(computeStringHash32("walk"), std::move(clip));
    clips.emplace_back(computeStringHash32("idle"), std::make_unique<pomdogschemas::Skeletal2DClipT>());
    std::ranges::sort(clips, {}, [](const auto& entry) { return entry.first; });

    pomdogschemas::Skeletal2DClipSetT clipSet;
    for (auto& [key, value] : clips) {
        clipSet.keys.push_back(key);
        clipSet.clips.push_back(std::move(value));
    }
    return clipSet;
}

/// Returns the clip "walk" of a clip set that makeClipSet() returned.
[[nodiscard]] pomdogschemas::Skeletal2DClipT&
getWalkClip(pomdogschemas::Skeletal2DClipSetT& clipSet)
{
    const auto iter = std::find(clipSet.keys.begin(), clipSet.keys.end(), computeStringHash32("walk"));
    REQUIRE(iter != clipSet.keys.end());
    const auto index = static_cast<std::size_t>(std::distance(clipSet.keys.begin(), iter));
    return *clipSet.clips[index];
}

[[nodiscard]] pomdogschemas::Skeletal2DAnimGraphNode
makeClipNode(std::string_view clipName)
{
    return pomdogschemas::Skeletal2DAnimGraphNode{computeStringHash32(clipName), u16(0), u16(0), u16(0), pomdogschemas::Skeletal2DAnimGraphNodeType_Clip};
}

[[nodiscard]] pomdogschemas::Skeletal2DAnimGraphNode
makeLerpNode(u16 input0, u16 input1, u16 parameter)
{
    return pomdogschemas::Skeletal2DAnimGraphNode{0, input0, input1, parameter, pomdogschemas::Skeletal2DAnimGraphNodeType_Lerp};
}

[[nodiscard]] pomdogschemas::Skeletal2DAnimGraphState
makeState(std::string_view name, u16 tree)
{
    return pomdogschemas::Skeletal2DAnimGraphState{computeStringHash32(name), tree};
}

/// Returns a graph for the clips of makeClipSet(). The state "move" blends
/// the clips "walk" and "idle" by the parameter "speed", and the state "rest"
/// plays the clip "idle".
[[nodiscard]] pomdogschemas::Skeletal2DAnimGraphT
makeGraph()
{
    pomdogschemas::Skeletal2DAnimGraphT graph;
    graph.nodes.push_back(makeClipNode("walk"));
    graph.nodes.push_back(makeClipNode("idle"));
    graph.nodes.push_back(makeLerpNode(0, 1, 0));
    graph.states.push_back(makeState("move", 2));
    graph.states.push_back(makeState("rest", 1));
    graph.parameters.push_back(computeStringHash32("speed"));
    return graph;
}

} // namespace

TEST_CASE("loadSkeletonData")
{
    SUBCASE("loads the joints that CreateSkeleton links")
    {
        const auto loaded = load(makeSkeleton());
        REQUIRE(loaded.err == nullptr);
        REQUIRE(loaded.data != nullptr);

        const auto skeleton = pomdog::spine::CreateSkeleton(*loaded.data);
        REQUIRE(skeleton.JointCount() == 4);

        const auto& root = skeleton.Root();
        REQUIRE(*root.Index == 0);
        REQUIRE_FALSE(root.Parent);
        REQUIRE(*root.FirstChild == 1);
        REQUIRE_FALSE(root.Sibling);

        const auto& joint1 = skeleton.Joints(JointIndex{u8(1)});
        REQUIRE(*joint1.Parent == 0);
        REQUIRE(*joint1.FirstChild == 3);
        REQUIRE(*joint1.Sibling == 2);
        REQUIRE(joint1.BindPose.Translate.x == doctest::Approx(10.0f));
        REQUIRE(joint1.BindPose.Translate.y == doctest::Approx(20.0f));
        REQUIRE(joint1.BindPose.Rotation.get() == doctest::Approx(pomdog::math::PiOver2<f32>));
        REQUIRE(joint1.BindPose.Scale == doctest::Approx(1.0f));

        const auto& joint2 = skeleton.Joints(JointIndex{u8(2)});
        REQUIRE(*joint2.Parent == 0);
        REQUIRE_FALSE(joint2.FirstChild);
        REQUIRE_FALSE(joint2.Sibling);
        REQUIRE(joint2.BindPose.Scale == doctest::Approx(2.0f));

        const auto& joint3 = skeleton.Joints(JointIndex{u8(3)});
        REQUIRE(*joint3.Parent == 1);
        REQUIRE_FALSE(joint3.FirstChild);
        REQUIRE_FALSE(joint3.Sibling);

        // NOTE: The inverse bind pose undoes the global bind pose of a joint.
        const auto bindPose = pomdog::skeletal2d::SkeletonPose::CreateBindPose(skeleton);
        const auto globalPose = pomdog::skeletal2d::SkeletonHelper::ToGlobalPose(skeleton, bindPose);
        REQUIRE(globalPose.size() == 4);
        for (const auto& joint : skeleton) {
            const auto identity = globalPose[*joint.Index] * joint.InverseBindPose;
            REQUIRE(identity(0, 0) == doctest::Approx(1.0f));
            REQUIRE(identity(0, 1) == doctest::Approx(0.0f).epsilon(0.0001));
            REQUIRE(identity(1, 0) == doctest::Approx(0.0f).epsilon(0.0001));
            REQUIRE(identity(1, 1) == doctest::Approx(1.0f));
            REQUIRE(identity(2, 0) == doctest::Approx(0.0f).epsilon(0.0001));
            REQUIRE(identity(2, 1) == doctest::Approx(0.0f).epsilon(0.0001));
        }
    }
    SUBCASE("fails for a missing file")
    {
        auto [ctx, ctxErr] = pomdog::vfs::create();
        REQUIRE(ctxErr == nullptr);

        auto [data, err] = pomdog::spine::loadSkeletonData(ctx, "/tmp/pomdog_spine_loader_test_missing.skeleton_skel2d");
        REQUIRE(data == nullptr);
        REQUIRE(err != nullptr);
    }
    SUBCASE("rejects a file of another kind")
    {
        // NOTE: The file identifier tells a skin file from a skeleton file.
        const auto loaded = loadSkeletonBytes(pack(makeSkinSet()));
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects truncated bytes")
    {
        auto bytes = pack(makeSkeleton());
        bytes.resize(bytes.size() / 2);

        const auto loaded = loadSkeletonBytes(bytes);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a skeleton without joints")
    {
        const auto loaded = load(pomdogschemas::Skeletal2DSkeletonT{});
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a first joint that has a parent")
    {
        auto skeleton = makeSkeleton();
        skeleton.joints[0] = pomdogschemas::Skeletal2DJoint{0.0f, 0.0f, 0.0f, 1.0f, u8(0)};

        const auto loaded = load(skeleton);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a joint that precedes its parent")
    {
        auto skeleton = makeSkeleton();
        skeleton.joints[1] = pomdogschemas::Skeletal2DJoint{10.0f, 20.0f, 0.0f, 1.0f, u8(3)};

        const auto loaded = load(skeleton);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a second joint without a parent")
    {
        auto skeleton = makeSkeleton();
        skeleton.joints[2] = pomdogschemas::Skeletal2DJoint{-5.0f, 0.0f, 0.0f, 2.0f, noJoint};

        const auto loaded = load(skeleton);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
}

TEST_CASE("loadSkinData")
{
    SUBCASE("finds a skin by the hash of its name")
    {
        const auto loaded = load(makeSkinSet());
        REQUIRE(loaded.err == nullptr);
        REQUIRE(loaded.data != nullptr);

        REQUIRE(loaded.data->findSkin(computeStringHash32("default")) != nullptr);
        REQUIRE(loaded.data->findSkin(computeStringHash32("missing")) == nullptr);
    }
    SUBCASE("loads a file without skins")
    {
        const auto loaded = load(pomdogschemas::Skeletal2DSkinSetT{});
        REQUIRE(loaded.err == nullptr);
        REQUIRE(loaded.data != nullptr);
        REQUIRE(loaded.data->findSkin(computeStringHash32("default")) == nullptr);
    }
    SUBCASE("rejects keys that are not in ascending order")
    {
        auto skinSet = makeSkinSet();
        skinSet.keys = {2, 1};
        skinSet.skins.push_back(std::make_unique<pomdogschemas::Skeletal2DSkinT>());

        const auto loaded = load(skinSet);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects keys and skins of different lengths")
    {
        auto skinSet = makeSkinSet();
        skinSet.keys.push_back(std::numeric_limits<u32>::max());

        const auto loaded = load(skinSet);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a slot that follows no joint")
    {
        auto skinSet = makeSkinSet();
        skinSet.skins[0]->slots[0] = pomdogschemas::Skeletal2DSkinSlot{computeStringHash32("body"), computeStringHash32("image"), 7.0f, 8.0f, 1.0f, 2.0f, 0.5f, noMesh, noJoint};

        const auto loaded = load(skinSet);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a slot whose mesh is out of range")
    {
        auto skinSet = makeSkinSet();
        skinSet.skins[0]->slots[1] = pomdogschemas::Skeletal2DSkinSlot{computeStringHash32("cloth"), computeStringHash32("cloth_mesh"), 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, u16(1), u8(2)};

        const auto loaded = load(skinSet);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a mesh index that is out of range")
    {
        auto skinSet = makeSkinSet();
        skinSet.skins[0]->meshes[0]->indices = {0, 1, 3};

        const auto loaded = load(skinSet);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects mesh indices that do not form triangles")
    {
        auto skinSet = makeSkinSet();
        skinSet.skins[0]->meshes[0]->indices = {0, 1};

        const auto loaded = load(skinSet);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a mesh vertex that follows no joint")
    {
        auto skinSet = makeSkinSet();
        skinSet.skins[0]->meshes[0]->vertices[1] = pomdogschemas::Skeletal2DMeshVertex{3.0f, 4.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, noJoint, noJoint, noJoint, noJoint};

        const auto loaded = load(skinSet);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
}

TEST_CASE("CreateSkin")
{
    const auto loaded = load(makeSkinSet());
    REQUIRE(loaded.err == nullptr);

    SUBCASE("creates the slots that show regions")
    {
        const auto atlas = makeAtlas();
        auto [skin, err] = pomdog::spine::CreateSkin(*loaded.data, *atlas, "default");
        REQUIRE(err == nullptr);
        REQUIRE(skin != nullptr);

        // NOTE: The slot "cloth" shows a mesh, which a sprite cannot show.
        REQUIRE(skin->GetSlots().size() == 1);

        const auto& slot = skin->GetSlots()[0];
        REQUIRE(*slot.JointIndex == 1);
        REQUIRE(slot.HashID == computeStringHash32("body"));
        REQUIRE(slot.DrawOrder == 0);
        REQUIRE(slot.Translate.x == doctest::Approx(7.0f));
        REQUIRE(slot.Translate.y == doctest::Approx(8.0f));
        REQUIRE(slot.Scale.x == doctest::Approx(1.0f));
        REQUIRE(slot.Scale.y == doctest::Approx(2.0f));
        REQUIRE(slot.Rotation.get() == doctest::Approx(0.5f));
        REQUIRE(slot.Subrect.x == 10);
        REQUIRE(slot.Subrect.y == 20);
        REQUIRE(slot.Subrect.width == 30);
        REQUIRE(slot.Subrect.height == 40);
        REQUIRE(slot.Origin.x == doctest::Approx(16.0f / 30.0f));
        REQUIRE(slot.Origin.y == doctest::Approx(0.5f));
    }
    SUBCASE("fails for an unknown skin")
    {
        const auto atlas = makeAtlas();
        auto [skin, err] = pomdog::spine::CreateSkin(*loaded.data, *atlas, "missing");
        REQUIRE(skin == nullptr);
        REQUIRE(err != nullptr);
    }
    SUBCASE("fails when the atlas has no region of an attachment")
    {
        FakeTextureAtlas atlas;
        atlas.add("cloth_mesh", makeRegion(100, 120));

        auto [skin, err] = pomdog::spine::CreateSkin(*loaded.data, atlas, "default");
        REQUIRE(skin == nullptr);
        REQUIRE(err != nullptr);
    }
}

TEST_CASE("CreateSkinnedMesh")
{
    const auto loaded = load(makeSkinSet());
    REQUIRE(loaded.err == nullptr);

    const auto atlas = makeAtlas();
    const auto textureSize = Vector2{200.0f, 400.0f};
    const std::vector<Matrix3x2> bindPoses(4, Matrix3x2::createIdentity());

    SUBCASE("creates a quad for a region and vertices for a mesh")
    {
        auto [mesh, err] = pomdog::spine::CreateSkinnedMesh(bindPoses, *loaded.data, *atlas, textureSize, "default");
        REQUIRE(err == nullptr);

        REQUIRE(mesh.MeshParts.size() == 2);
        REQUIRE(mesh.MeshParts[0].VertexOffset == 0);
        REQUIRE(mesh.MeshParts[0].VertexCount == 4);
        REQUIRE(mesh.MeshParts[0].StartIndex == 0);
        REQUIRE(mesh.MeshParts[0].PrimitiveCount == 2);
        REQUIRE(mesh.MeshParts[1].VertexOffset == 4);
        REQUIRE(mesh.MeshParts[1].VertexCount == 3);
        REQUIRE(mesh.MeshParts[1].StartIndex == 6);
        REQUIRE(mesh.MeshParts[1].PrimitiveCount == 1);

        REQUIRE(mesh.Vertices.size() == 7);
        REQUIRE(mesh.Indices == std::vector<std::uint16_t>{0, 1, 2, 2, 3, 0, 4, 5, 6});

        // NOTE: The quad follows the joint of its slot.
        const auto& quadVertex = mesh.Vertices[0];
        REQUIRE(quadVertex.Joints[0] == 1);
        REQUIRE(quadVertex.Joints[1] == -1);
        REQUIRE(quadVertex.Weights[0] == doctest::Approx(1.0f));
        REQUIRE(quadVertex.PositionTextureCoord.z == doctest::Approx(10.0f / 200.0f));
        REQUIRE(quadVertex.PositionTextureCoord.w == doctest::Approx((20.0f + 40.0f) / 400.0f));

        const auto& meshVertex = mesh.Vertices[4];
        REQUIRE(meshVertex.PositionTextureCoord.x == doctest::Approx(1.0f));
        REQUIRE(meshVertex.PositionTextureCoord.y == doctest::Approx(2.0f));
        REQUIRE(meshVertex.PositionTextureCoord.z == doctest::Approx(100.0f / 200.0f));
        REQUIRE(meshVertex.PositionTextureCoord.w == doctest::Approx(120.0f / 400.0f));
        REQUIRE(meshVertex.Joints[0] == 2);
        REQUIRE(meshVertex.Joints[1] == 1);
        REQUIRE(meshVertex.Joints[2] == -1);
        REQUIRE(meshVertex.Joints[3] == -1);
        REQUIRE(meshVertex.Weights[0] == doctest::Approx(0.75f));
        REQUIRE(meshVertex.Weights[1] == doctest::Approx(0.25f));
        REQUIRE(meshVertex.Weights[2] == doctest::Approx(0.0f));
        REQUIRE(meshVertex.Weights[3] == doctest::Approx(0.0f));

        // NOTE: Texture coordinates of a mesh span the region of its attachment.
        const auto& lastVertex = mesh.Vertices[6];
        REQUIRE(lastVertex.PositionTextureCoord.z == doctest::Approx((100.0f + 0.5f * 30.0f) / 200.0f));
        REQUIRE(lastVertex.PositionTextureCoord.w == doctest::Approx((120.0f + 40.0f) / 400.0f));
    }
    SUBCASE("fails for an unknown skin")
    {
        auto [mesh, err] = pomdog::spine::CreateSkinnedMesh(bindPoses, *loaded.data, *atlas, textureSize, "missing");
        REQUIRE(err != nullptr);
    }
    SUBCASE("fails when the skeleton has fewer joints than the skin refers to")
    {
        const std::vector<Matrix3x2> fewBindPoses(1, Matrix3x2::createIdentity());
        auto [mesh, err] = pomdog::spine::CreateSkinnedMesh(fewBindPoses, *loaded.data, *atlas, textureSize, "default");
        REQUIRE(err != nullptr);
    }
    SUBCASE("fails when the atlas has no region of an attachment")
    {
        FakeTextureAtlas emptyAtlas;
        auto [mesh, err] = pomdog::spine::CreateSkinnedMesh(bindPoses, *loaded.data, emptyAtlas, textureSize, "default");
        REQUIRE(err != nullptr);
    }
}

TEST_CASE("loadAnimationClipData")
{
    SUBCASE("finds a clip by the hash of its name")
    {
        const auto loaded = load(makeClipSet());
        REQUIRE(loaded.err == nullptr);
        REQUIRE(loaded.data != nullptr);

        REQUIRE(loaded.data->findClip(computeStringHash32("walk")) != nullptr);
        REQUIRE(loaded.data->findClip(computeStringHash32("idle")) != nullptr);
        REQUIRE(loaded.data->findClip(computeStringHash32("missing")) == nullptr);
    }
    SUBCASE("rejects keys that are not in ascending order")
    {
        auto clipSet = makeClipSet();
        std::swap(clipSet.keys[0], clipSet.keys[1]);

        const auto loaded = load(clipSet);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a track that animates no joint")
    {
        auto clipSet = makeClipSet();
        getWalkClip(clipSet).rotation_tracks[0]->joint = noJoint;

        const auto loaded = load(clipSet);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a track without keys")
    {
        auto clipSet = makeClipSet();
        getWalkClip(clipSet).scale_tracks[0]->keys.clear();

        const auto loaded = load(clipSet);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects keys that are not in ascending order of time")
    {
        auto clipSet = makeClipSet();
        getWalkClip(clipSet).rotation_tracks[0]->keys[0] = pomdogschemas::Skeletal2DRotationKey{0.5f, 0.0f};
        getWalkClip(clipSet).rotation_tracks[0]->keys[1] = pomdogschemas::Skeletal2DRotationKey{0.25f, 1.0f};

        const auto loaded = load(clipSet);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a negative time")
    {
        auto clipSet = makeClipSet();
        getWalkClip(clipSet).attachment_tracks[0]->keys[0] = pomdogschemas::Skeletal2DAttachmentKey{-1.0f, computeStringHash32("image")};

        const auto loaded = load(clipSet);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a time that is not a number")
    {
        auto clipSet = makeClipSet();
        getWalkClip(clipSet).translation_tracks[0]->keys[0] = pomdogschemas::Skeletal2DTranslationKey{std::numeric_limits<f32>::quiet_NaN(), 3.0f, 4.0f};

        const auto loaded = load(clipSet);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a rotation out of range")
    {
        auto clipSet = makeClipSet();
        getWalkClip(clipSet).rotation_tracks[0]->keys[1] = pomdogschemas::Skeletal2DRotationKey{1.0f, 3.5f};

        const auto loaded = load(clipSet);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a scale out of range")
    {
        auto clipSet = makeClipSet();
        getWalkClip(clipSet).scale_tracks[0]->keys[0] = pomdogschemas::Skeletal2DScaleKey{0.0f, -20.5f};

        const auto loaded = load(clipSet);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a translation out of range")
    {
        auto clipSet = makeClipSet();
        getWalkClip(clipSet).translation_tracks[0]->keys[0] = pomdogschemas::Skeletal2DTranslationKey{0.0f, 3.0f, 1024.5f};

        const auto loaded = load(clipSet);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a value that is not a number")
    {
        auto clipSet = makeClipSet();
        getWalkClip(clipSet).translation_tracks[0]->keys[0] = pomdogschemas::Skeletal2DTranslationKey{0.0f, std::numeric_limits<f32>::quiet_NaN(), 4.0f};

        const auto loaded = load(clipSet);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
}

TEST_CASE("CreateAnimationClip")
{
    const auto loadedSkeleton = load(makeSkeleton());
    REQUIRE(loadedSkeleton.err == nullptr);
    const auto skeleton = pomdog::spine::CreateSkeleton(*loadedSkeleton.data);

    const auto loaded = load(makeClipSet());
    REQUIRE(loaded.err == nullptr);

    SUBCASE("creates tracks that animate the joints")
    {
        auto [clip, err] = pomdog::spine::CreateAnimationClip(*loaded.data, nullptr, "walk");
        REQUIRE(err == nullptr);
        REQUIRE(clip != nullptr);
        REQUIRE(clip->GetLength().count() == doctest::Approx(1.0f));

        auto pose = pomdog::skeletal2d::SkeletonPose::CreateBindPose(skeleton);
        clip->Apply(AnimationTimeInterval{0.5f}, skeleton, pose, nullptr);

        // NOTE: The key values are relative to the bind pose, and the tracks
        // store them in fixed point.
        REQUIRE(pose.JointPoses[1].Rotation.get() == doctest::Approx(pomdog::math::PiOver2<f32> + 0.5f).epsilon(0.001));
        REQUIRE(pose.JointPoses[1].Translate.x == doctest::Approx(10.0f + 3.0f).epsilon(0.001));
        REQUIRE(pose.JointPoses[1].Translate.y == doctest::Approx(20.0f + 4.0f).epsilon(0.001));
        REQUIRE(pose.JointPoses[2].Scale == doctest::Approx(2.0f * 2.0f).epsilon(0.001));

        // NOTE: Joints without tracks keep their pose.
        REQUIRE(pose.JointPoses[3].Rotation.get() == doctest::Approx(-0.5f));
    }
    SUBCASE("creates a clip without tracks")
    {
        auto [clip, err] = pomdog::spine::CreateAnimationClip(*loaded.data, nullptr, "idle");
        REQUIRE(err == nullptr);
        REQUIRE(clip != nullptr);
    }
    SUBCASE("switches the attachment of a slot of the skin")
    {
        const auto loadedSkin = load(makeSkinSet());
        REQUIRE(loadedSkin.err == nullptr);

        const auto atlas = makeAtlas();
        auto [skin, skinErr] = pomdog::spine::CreateSkin(*loadedSkin.data, *atlas, "default");
        REQUIRE(skinErr == nullptr);

        auto [clip, err] = pomdog::spine::CreateAnimationClip(*loaded.data, atlas, "walk");
        REQUIRE(err == nullptr);
        REQUIRE(clip != nullptr);

        // NOTE: The track finds the slot by the hash of its name, which the
        // skin file and the clip file share.
        auto pose = pomdog::skeletal2d::SkeletonPose::CreateBindPose(skeleton);
        clip->Apply(AnimationTimeInterval{0.75f}, skeleton, pose, skin.get());
        REQUIRE(skin->GetSlots()[0].Subrect.x == 50);
        REQUIRE(skin->GetSlots()[0].Subrect.y == 60);

        clip->Apply(AnimationTimeInterval{0.25f}, skeleton, pose, skin.get());
        REQUIRE(skin->GetSlots()[0].Subrect.x == 10);
        REQUIRE(skin->GetSlots()[0].Subrect.y == 20);
    }
    SUBCASE("fails for an unknown clip")
    {
        auto [clip, err] = pomdog::spine::CreateAnimationClip(*loaded.data, nullptr, "missing");
        REQUIRE(clip == nullptr);
        REQUIRE(err != nullptr);
    }
    SUBCASE("fails when the atlas has no region of an attachment")
    {
        auto atlas = std::make_shared<FakeTextureAtlas>();
        atlas->add("image", makeRegion(10, 20));

        auto [clip, err] = pomdog::spine::CreateAnimationClip(*loaded.data, atlas, "walk");
        REQUIRE(clip == nullptr);
        REQUIRE(err != nullptr);
    }
}

TEST_CASE("loadAnimationGraph")
{
    const auto loadedSkeleton = load(makeSkeleton());
    REQUIRE(loadedSkeleton.err == nullptr);
    const auto skeleton = std::make_shared<pomdog::skeletal2d::Skeleton>(pomdog::spine::CreateSkeleton(*loadedSkeleton.data));

    const auto loadedClips = load(makeClipSet());
    REQUIRE(loadedClips.err == nullptr);
    const auto& clipData = *loadedClips.data;

    SUBCASE("creates the states in the order of the file")
    {
        const auto loaded = load(makeGraph(), clipData);
        REQUIRE(loaded.err == nullptr);
        REQUIRE(loaded.data != nullptr);

        const auto& graph = *loaded.data;
        REQUIRE(graph.States.size() == 2);
        REQUIRE(graph.States[0].Key == computeStringHash32("move"));
        REQUIRE(graph.States[0].Tree != nullptr);
        REQUIRE(graph.States[0].Tree->GetLength().count() == doctest::Approx(1.0f));
        REQUIRE(graph.States[1].Key == computeStringHash32("rest"));
        REQUIRE(graph.States[1].Tree != nullptr);
        REQUIRE(graph.States[1].Tree->GetLength().count() == doctest::Approx(0.0f));

        REQUIRE(graph.Inputs.size() == 1);
        REQUIRE(graph.Inputs[0].Key == computeStringHash32("speed"));
        REQUIRE(graph.Inputs[0].Type == pomdog::skeletal2d::AnimationBlendInputType::Float);
        REQUIRE(graph.FindParameter(computeStringHash32("speed")) == std::optional<std::uint16_t>{std::uint16_t(0)});
        REQUIRE(graph.FindParameter(computeStringHash32("missing")) == std::nullopt);
    }
    SUBCASE("blends the inputs of a lerp node by its parameter")
    {
        const auto loaded = load(makeGraph(), clipData);
        REQUIRE(loaded.err == nullptr);

        // NOTE: After half a second, the clip "walk" has rotated joint 1 by
        // half a radian, and the clip "idle" leaves it in the bind pose.
        const auto bindRotation = pomdog::math::PiOver2<f32>;
        const auto rotationAt = [&](f32 speed) -> f32 {
            auto pose = std::make_shared<pomdog::skeletal2d::SkeletonPose>(pomdog::skeletal2d::SkeletonPose::CreateBindPose(*skeleton));
            pomdog::skeletal2d::Animator animator{skeleton, pose, loaded.data};
            REQUIRE(animator.GetCurrentStateKey() == computeStringHash32("move"));

            animator.SetFloat(computeStringHash32("speed"), speed);
            animator.Update(Duration{0.5});
            return pose->JointPoses[1].Rotation.get();
        };

        REQUIRE(rotationAt(0.0f) == doctest::Approx(bindRotation + 0.5f).epsilon(0.001));
        REQUIRE(rotationAt(0.5f) == doctest::Approx(bindRotation + 0.25f).epsilon(0.001));
        REQUIRE(rotationAt(1.0f) == doctest::Approx(bindRotation).epsilon(0.001));
    }
    SUBCASE("plays a state by its key")
    {
        const auto loaded = load(makeGraph(), clipData);
        REQUIRE(loaded.err == nullptr);

        auto pose = std::make_shared<pomdog::skeletal2d::SkeletonPose>(pomdog::skeletal2d::SkeletonPose::CreateBindPose(*skeleton));
        pomdog::skeletal2d::Animator animator{skeleton, pose, loaded.data};
        animator.Play(computeStringHash32("rest"));
        REQUIRE(animator.GetCurrentStateKey() == computeStringHash32("rest"));

        animator.Update(Duration{0.5});
        REQUIRE(pose->JointPoses[1].Rotation.get() == doctest::Approx(pomdog::math::PiOver2<f32>));
        REQUIRE(pose->JointPoses[2].Scale == doctest::Approx(2.0f));
    }
    SUBCASE("creates a copy of a node for each reference to it")
    {
        // NOTE: Each lerp node takes the previous node twice, so the tree of
        // the last node has 2^10 - 1 = 1023 nodes, which is within the limit.
        pomdogschemas::Skeletal2DAnimGraphT graph;
        graph.nodes.push_back(makeClipNode("idle"));
        for (u16 index = 1; index <= 9; ++index) {
            graph.nodes.push_back(makeLerpNode(static_cast<u16>(index - 1), static_cast<u16>(index - 1), 0));
        }
        graph.states.push_back(makeState("deep", 9));
        graph.parameters.push_back(computeStringHash32("speed"));

        const auto loaded = load(graph, clipData);
        REQUIRE(loaded.err == nullptr);
        REQUIRE(loaded.data->States[0].Tree != nullptr);
    }
    SUBCASE("rejects a tree that has too many nodes")
    {
        // NOTE: One more level makes a tree of 2^11 - 1 = 2047 nodes.
        pomdogschemas::Skeletal2DAnimGraphT graph;
        graph.nodes.push_back(makeClipNode("idle"));
        for (u16 index = 1; index <= 10; ++index) {
            graph.nodes.push_back(makeLerpNode(static_cast<u16>(index - 1), static_cast<u16>(index - 1), 0));
        }
        graph.states.push_back(makeState("deep", 10));
        graph.parameters.push_back(computeStringHash32("speed"));

        const auto loaded = load(graph, clipData);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("fails when the clip file has no clip of a clip node")
    {
        auto graph = makeGraph();
        graph.nodes[1] = makeClipNode("missing");

        const auto loaded = load(graph, clipData);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("fails for a missing file")
    {
        auto [ctx, ctxErr] = pomdog::vfs::create();
        REQUIRE(ctxErr == nullptr);

        auto [graph, err] = pomdog::spine::loadAnimationGraph(ctx, clipData, "/tmp/pomdog_spine_loader_test_missing.animgraph_skel2d");
        REQUIRE(graph == nullptr);
        REQUIRE(err != nullptr);
    }
    SUBCASE("rejects a file of another kind")
    {
        const auto loaded = loadGraphBytes(pack(makeClipSet()), clipData);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a graph without states")
    {
        auto graph = makeGraph();
        graph.states.clear();

        const auto loaded = load(graph, clipData);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects two states with the same key")
    {
        auto graph = makeGraph();
        graph.states[1] = makeState("move", 1);

        const auto loaded = load(graph, clipData);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects two parameters with the same key")
    {
        auto graph = makeGraph();
        graph.parameters.push_back(computeStringHash32("speed"));

        const auto loaded = load(graph, clipData);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a state whose tree is out of range")
    {
        auto graph = makeGraph();
        graph.states[0] = makeState("move", 3);

        const auto loaded = load(graph, clipData);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a lerp node that does not follow its inputs")
    {
        auto graph = makeGraph();
        graph.nodes[2] = makeLerpNode(0, 2, 0);

        const auto loaded = load(graph, clipData);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a lerp node whose parameter is out of range")
    {
        auto graph = makeGraph();
        graph.nodes[2] = makeLerpNode(0, 1, 1);

        const auto loaded = load(graph, clipData);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
    SUBCASE("rejects a node of an unknown type")
    {
        auto graph = makeGraph();
        graph.nodes[0] = pomdogschemas::Skeletal2DAnimGraphNode{0, u16(0), u16(0), u16(0), static_cast<pomdogschemas::Skeletal2DAnimGraphNodeType>(7)};

        const auto loaded = load(graph, clipData);
        REQUIRE(loaded.data == nullptr);
        REQUIRE(loaded.err != nullptr);
    }
}
