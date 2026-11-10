// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/spine/skinned_mesh_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/skeleton_pose.h"
#include "pomdog/experimental/skeletal2d/skinned_mesh.h"
#include "pomdog/experimental/spine/skin_data.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/utility/string_hash32.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/skeletal2d_skin_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <limits>
#include <tuple>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::spine {
namespace {

using skeletal2d::SkinnedMeshPart;
using skeletal2d::SkinnedVertex;

// NOTE: A joint index of 255 means no joint, and a mesh index of 65535 means
// that the slot shows a region attachment.
constexpr u8 noJoint = 255;
constexpr u16 noMesh = 65535;

struct SkinnedMeshSlot final {
    std::vector<skeletal2d::SkinnedVertex> Vertices;
    std::vector<std::uint16_t> Indices;
};

[[nodiscard]] std::int32_t
ToJointIndex(u8 joint) noexcept
{
    if (joint == noJoint) {
        return -1;
    }
    return joint;
}

SkinnedMeshSlot
CreateSkinnedMeshSlot(
    const pomdogschemas::Skeletal2DSkinSlot& skinSlot,
    const TextureRegion& textureRegion,
    const Vector2& textureSize,
    const std::vector<Matrix3x2>& bindPosesInGlobal)
{
    SkinnedMeshSlot slot;

    ///@todo Not implemented

    std::array<float, 4> weights = {{1.0f, 0.0f, 0.0f, 0.0f}};
    std::array<std::int32_t, 4> joints = {{skinSlot.joint(), -1, -1, -1}};

    slot.Vertices = {
        {Vector4{0.0f, 0.0f, 0.0f, 1.0f}, weights, joints},
        {Vector4{0.0f, 1.0f, 0.0f, 0.0f}, weights, joints},
        {Vector4{1.0f, 1.0f, 1.0f, 0.0f}, weights, joints},
        {Vector4{1.0f, 0.0f, 1.0f, 1.0f}, weights, joints},
    };

    slot.Indices = {0, 1, 2, 2, 3, 0};

    auto origin = Vector2{
        static_cast<f32>(textureRegion.subrectWidth / 2 - textureRegion.xOffset) / textureRegion.subrectWidth,
        static_cast<f32>(textureRegion.subrectHeight / 2 - textureRegion.yOffset) / textureRegion.subrectHeight,
    };

    auto size = Vector2{
        static_cast<f32>(textureRegion.subrectWidth),
        static_cast<f32>(textureRegion.subrectHeight),
    };

    constexpr bool isTextureRegionRotated = false;

    if (isTextureRegionRotated) {
        std::swap(size.x, size.y);
        std::swap(origin.x, origin.y);
    }

    auto scaling = Matrix3x2::createScale(Vector2{skinSlot.scale_x(), skinSlot.scale_y()} * size);
    auto rotate = Matrix3x2::createRotation(skinSlot.rotation());
    if (isTextureRegionRotated) {
        rotate = Matrix3x2::createRotation(-math::PiOver2<float>) * rotate;
    }
    const auto translate = Matrix3x2::createTranslation(Vector2{skinSlot.translate_x(), skinSlot.translate_y()});

    POMDOG_ASSERT(skinSlot.joint() < bindPosesInGlobal.size());
    Matrix3x2 transform = scaling * rotate * translate * bindPosesInGlobal[skinSlot.joint()];

    for (auto& vertex : slot.Vertices) {
        auto position = math::transform(Vector2(vertex.PositionTextureCoord.x, vertex.PositionTextureCoord.y) - origin, transform);

        vertex.PositionTextureCoord.x = position.x;
        vertex.PositionTextureCoord.y = position.y;
    }

    for (auto& vertex : slot.Vertices) {
        auto position =
            Vector2{vertex.PositionTextureCoord.z, vertex.PositionTextureCoord.w} * size +
            Vector2{static_cast<f32>(textureRegion.subrectX), static_cast<f32>(textureRegion.subrectY)};

        POMDOG_ASSERT(textureSize.x > 0);
        POMDOG_ASSERT(textureSize.y > 0);
        position = position / textureSize;

        vertex.PositionTextureCoord.z = position.x;
        vertex.PositionTextureCoord.w = position.y;
    }

    return slot;
}

[[nodiscard]] std::tuple<SkinnedMeshSlot, std::unique_ptr<Error>>
CreateSkinnedMeshSlot(
    const pomdogschemas::Skeletal2DMesh& mesh,
    const TextureRegion& textureRegion,
    const Vector2& textureSize,
    const std::vector<Matrix3x2>& bindPosesInGlobal)
{
    SkinnedMeshSlot meshSlot;
    meshSlot.Vertices.reserve(mesh.vertices()->size());

    for (const auto* source : *mesh.vertices()) {
        SkinnedVertex vertex;

        // NOTE: The position is in the local space of the first joint.
        if (source->joint0() >= bindPosesInGlobal.size()) {
            auto err = errors::make(pomdog::format("mesh follows joint {}, but the skeleton has {} joints", source->joint0(), bindPosesInGlobal.size()));
            return std::make_tuple(SkinnedMeshSlot{}, std::move(err));
        }
        auto position = math::transform(Vector2{source->position_x(), source->position_y()}, bindPosesInGlobal[source->joint0()]);

        auto originInUV = Vector2{
            static_cast<f32>(textureRegion.subrectX),
            static_cast<f32>(textureRegion.subrectY),
        };
        auto sizeInUV = Vector2{
            static_cast<f32>(textureRegion.subrectWidth),
            static_cast<f32>(textureRegion.subrectHeight),
        };

        constexpr bool isTextureRegionRotated = false;

        const auto textureCoordinate = Vector2{source->texcoord_u(), source->texcoord_v()};
        Vector2 textureCoordInUV = isTextureRegionRotated
                                       ? Vector2{
                                             textureCoordinate.y * sizeInUV.y,
                                             (1 - textureCoordinate.x) * sizeInUV.x,
                                         }
                                       : textureCoordinate * sizeInUV;

        auto textureCoord = (originInUV + textureCoordInUV) / textureSize;

        vertex.PositionTextureCoord = {
            position.x,
            position.y,
            textureCoord.x,
            textureCoord.y,
        };

        vertex.Joints = {{
            ToJointIndex(source->joint0()),
            ToJointIndex(source->joint1()),
            ToJointIndex(source->joint2()),
            ToJointIndex(source->joint3()),
        }};
        vertex.Weights = {{
            source->weight0(),
            source->weight1(),
            source->weight2(),
            source->weight3(),
        }};

        meshSlot.Vertices.push_back(std::move(vertex));
    }

    meshSlot.Indices.assign(mesh.indices()->begin(), mesh.indices()->end());
    return std::make_tuple(std::move(meshSlot), nullptr);
}

[[nodiscard]] std::tuple<skeletal2d::SkinnedMesh, std::unique_ptr<Error>>
CreateVertices(
    const pomdogschemas::Skeletal2DSkin& skin,
    const TextureAtlas& textureAtlas,
    const Vector2& textureSize,
    const std::vector<Matrix3x2>& bindPosesInGlobal)
{
    skeletal2d::SkinnedMesh result;

    const auto* skinSlots = skin.slots();
    if (skinSlots == nullptr) {
        return std::make_tuple(std::move(result), nullptr);
    }
    result.MeshParts.reserve(skinSlots->size());

    // NOTE: The slots are in draw order.
    for (const auto* skinSlot : *skinSlots) {
        // NOTE: The key of an attachment is also the key of its region in the texture atlas.
        auto optIdx = textureAtlas.findRegionByKey(skinSlot->attachment_key());
        if (!optIdx.has_value()) {
            auto err = errors::make(pomdog::format("cannot find attachment {:#010x} in the texture atlas", skinSlot->attachment_key()));
            return std::make_tuple(skeletal2d::SkinnedMesh{}, std::move(err));
        }
        const auto textureRegion = textureAtlas.getRegion(*optIdx);

        SkinnedMeshSlot slot;
        if (skinSlot->mesh() != noMesh) {
            // NOTE: SkinData has verified that the slot refers to a mesh of the skin.
            auto [meshSlot, meshErr] = CreateSkinnedMeshSlot(*skin.meshes()->Get(skinSlot->mesh()), textureRegion, textureSize, bindPosesInGlobal);
            if (meshErr != nullptr) {
                return std::make_tuple(skeletal2d::SkinnedMesh{}, std::move(meshErr));
            }
            slot = std::move(meshSlot);
        }
        else {
            if (skinSlot->joint() >= bindPosesInGlobal.size()) {
                auto err = errors::make(pomdog::format("slot follows joint {}, but the skeleton has {} joints", skinSlot->joint(), bindPosesInGlobal.size()));
                return std::make_tuple(skeletal2d::SkinnedMesh{}, std::move(err));
            }
            slot = CreateSkinnedMeshSlot(*skinSlot, textureRegion, textureSize, bindPosesInGlobal);
        }

        // NOTE: The mesh parts and the indices address vertices with 16 bits.
        constexpr std::size_t maxCount = std::numeric_limits<std::uint16_t>::max();
        if ((result.Vertices.size() + slot.Vertices.size() > maxCount) ||
            (result.Indices.size() + slot.Indices.size() > maxCount)) {
            auto err = errors::make(pomdog::format("skin has more than {} vertices or indices", maxCount));
            return std::make_tuple(skeletal2d::SkinnedMesh{}, std::move(err));
        }

        const auto startIndex = static_cast<std::uint16_t>(result.Vertices.size());

        SkinnedMeshPart meshPart;
        meshPart.VertexOffset = startIndex;
        meshPart.VertexCount = static_cast<std::uint16_t>(slot.Vertices.size());
        meshPart.StartIndex = static_cast<std::uint16_t>(result.Indices.size());

        POMDOG_ASSERT(slot.Indices.size() % 3 == 0);
        meshPart.PrimitiveCount = static_cast<std::uint16_t>(slot.Indices.size()) / 3;
        result.MeshParts.push_back(std::move(meshPart));

        for (auto& index : slot.Indices) {
            index += startIndex;
        }

        // NOTE: concatenate two vectors
        result.Vertices.insert(std::end(result.Vertices), std::begin(slot.Vertices), std::end(slot.Vertices));
        result.Indices.insert(std::end(result.Indices), std::begin(slot.Indices), std::end(slot.Indices));
    }
    return std::make_tuple(std::move(result), nullptr);
}

} // namespace

std::tuple<skeletal2d::SkinnedMesh, std::unique_ptr<Error>>
CreateSkinnedMesh(
    const std::vector<Matrix3x2>& bindPosesInGlobal,
    const SkinData& skinData,
    const TextureAtlas& textureAtlas,
    const Vector2& textureSize,
    std::string_view skinName)
{
    const auto* skin = skinData.findSkin(computeStringHash32(skinName));
    if (skin == nullptr) {
        auto err = errors::make(pomdog::format("cannot find skin '{}'", skinName));
        return std::make_tuple(skeletal2d::SkinnedMesh{}, std::move(err));
    }

    auto [meshData, meshErr] = CreateVertices(*skin, textureAtlas, textureSize, bindPosesInGlobal);
    if (meshErr != nullptr) {
        auto wrapped = errors::wrap(std::move(meshErr), pomdog::format("failed to create skinned mesh of skin '{}'", skinName));
        return std::make_tuple(skeletal2d::SkinnedMesh{}, std::move(wrapped));
    }
    return std::make_tuple(std::move(meshData), nullptr);
}

} // namespace pomdog::spine
