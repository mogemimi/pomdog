// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/skinned_mesh_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/skeleton_pose.h"
#include "pomdog/experimental/skeletal2d/skin_data.h"
#include "pomdog/experimental/skeletal2d/skinned_mesh.h"
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

namespace pomdog::skeletal2d {
namespace {

// NOTE: A joint index of 255 means no joint, and a mesh index of 65535 means
// that the slot shows a region attachment.
constexpr u8 noJoint = 255;
constexpr u16 noMesh = 65535;

struct SkinnedMeshSlot final {
    std::vector<SkinnedVertex> vertices;
    std::vector<u16> indices;
};

[[nodiscard]] i32
toJointIndex(u8 joint) noexcept
{
    if (joint == noJoint) {
        return -1;
    }
    return joint;
}

SkinnedMeshSlot
createSkinnedMeshSlot(
    const pomdogschemas::Skeletal2DSkinSlot& skinSlot,
    const TextureRegion& textureRegion,
    const Vector2& textureSize,
    const std::vector<Matrix3x2>& bindPosesInGlobal)
{
    SkinnedMeshSlot slot;

    ///@todo Not implemented

    std::array<f32, 4> weights = {{1.0f, 0.0f, 0.0f, 0.0f}};
    std::array<i32, 4> joints = {{skinSlot.joint(), -1, -1, -1}};

    slot.vertices = {
        {Vector4{0.0f, 0.0f, 0.0f, 1.0f}, weights, joints},
        {Vector4{0.0f, 1.0f, 0.0f, 0.0f}, weights, joints},
        {Vector4{1.0f, 1.0f, 1.0f, 0.0f}, weights, joints},
        {Vector4{1.0f, 0.0f, 1.0f, 1.0f}, weights, joints},
    };

    slot.indices = {0, 1, 2, 2, 3, 0};

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
        rotate = Matrix3x2::createRotation(-math::PiOver2<f32>) * rotate;
    }
    const auto translate = Matrix3x2::createTranslation(Vector2{skinSlot.translate_x(), skinSlot.translate_y()});

    POMDOG_ASSERT(skinSlot.joint() < bindPosesInGlobal.size());
    Matrix3x2 transform = scaling * rotate * translate * bindPosesInGlobal[skinSlot.joint()];

    for (auto& vertex : slot.vertices) {
        auto position = math::transform(Vector2(vertex.positionTextureCoord.x, vertex.positionTextureCoord.y) - origin, transform);

        vertex.positionTextureCoord.x = position.x;
        vertex.positionTextureCoord.y = position.y;
    }

    for (auto& vertex : slot.vertices) {
        auto position =
            Vector2{vertex.positionTextureCoord.z, vertex.positionTextureCoord.w} * size +
            Vector2{static_cast<f32>(textureRegion.subrectX), static_cast<f32>(textureRegion.subrectY)};

        POMDOG_ASSERT(textureSize.x > 0);
        POMDOG_ASSERT(textureSize.y > 0);
        position = position / textureSize;

        vertex.positionTextureCoord.z = position.x;
        vertex.positionTextureCoord.w = position.y;
    }

    return slot;
}

[[nodiscard]] std::tuple<SkinnedMeshSlot, std::unique_ptr<Error>>
createSkinnedMeshSlot(
    const pomdogschemas::Skeletal2DMesh& mesh,
    const TextureRegion& textureRegion,
    const Vector2& textureSize,
    const std::vector<Matrix3x2>& bindPosesInGlobal)
{
    SkinnedMeshSlot meshSlot;
    meshSlot.vertices.reserve(mesh.vertices()->size());

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

        vertex.positionTextureCoord = {
            position.x,
            position.y,
            textureCoord.x,
            textureCoord.y,
        };

        vertex.joints = {{
            toJointIndex(source->joint0()),
            toJointIndex(source->joint1()),
            toJointIndex(source->joint2()),
            toJointIndex(source->joint3()),
        }};
        vertex.weights = {{
            source->weight0(),
            source->weight1(),
            source->weight2(),
            source->weight3(),
        }};

        meshSlot.vertices.push_back(std::move(vertex));
    }

    meshSlot.indices.assign(mesh.indices()->begin(), mesh.indices()->end());
    return std::make_tuple(std::move(meshSlot), nullptr);
}

[[nodiscard]] std::tuple<SkinnedMesh, std::unique_ptr<Error>>
createVertices(
    const pomdogschemas::Skeletal2DSkin& skin,
    const TextureAtlas& textureAtlas,
    const Vector2& textureSize,
    const std::vector<Matrix3x2>& bindPosesInGlobal)
{
    SkinnedMesh result;

    const auto* skinSlots = skin.slots();
    if (skinSlots == nullptr) {
        return std::make_tuple(std::move(result), nullptr);
    }
    result.meshParts.reserve(skinSlots->size());

    // NOTE: The slots are in draw order.
    for (const auto* skinSlot : *skinSlots) {
        // NOTE: The key of an attachment is also the key of its region in the texture atlas.
        auto optIdx = textureAtlas.findRegionByKey(skinSlot->attachment_key());
        if (!optIdx.has_value()) {
            auto err = errors::make(pomdog::format("cannot find attachment {:#010x} in the texture atlas", skinSlot->attachment_key()));
            return std::make_tuple(SkinnedMesh{}, std::move(err));
        }
        const auto textureRegion = textureAtlas.getRegion(*optIdx);

        SkinnedMeshSlot slot;
        if (skinSlot->mesh() != noMesh) {
            // NOTE: SkinData has verified that the slot refers to a mesh of the skin.
            auto [meshSlot, meshErr] = createSkinnedMeshSlot(*skin.meshes()->Get(skinSlot->mesh()), textureRegion, textureSize, bindPosesInGlobal);
            if (meshErr != nullptr) {
                return std::make_tuple(SkinnedMesh{}, std::move(meshErr));
            }
            slot = std::move(meshSlot);
        }
        else {
            if (skinSlot->joint() >= bindPosesInGlobal.size()) {
                auto err = errors::make(pomdog::format("slot follows joint {}, but the skeleton has {} joints", skinSlot->joint(), bindPosesInGlobal.size()));
                return std::make_tuple(SkinnedMesh{}, std::move(err));
            }
            slot = createSkinnedMeshSlot(*skinSlot, textureRegion, textureSize, bindPosesInGlobal);
        }

        // NOTE: The mesh parts and the indices address vertices with 16 bits.
        constexpr std::size_t maxCount = std::numeric_limits<u16>::max();
        if ((result.vertices.size() + slot.vertices.size() > maxCount) ||
            (result.indices.size() + slot.indices.size() > maxCount)) {
            auto err = errors::make(pomdog::format("skin has more than {} vertices or indices", maxCount));
            return std::make_tuple(SkinnedMesh{}, std::move(err));
        }

        const auto startIndex = static_cast<u16>(result.vertices.size());

        SkinnedMeshPart meshPart;
        meshPart.vertexOffset = startIndex;
        meshPart.vertexCount = static_cast<u16>(slot.vertices.size());
        meshPart.startIndex = static_cast<u16>(result.indices.size());

        POMDOG_ASSERT(slot.indices.size() % 3 == 0);
        meshPart.primitiveCount = static_cast<u16>(slot.indices.size()) / 3;
        result.meshParts.push_back(std::move(meshPart));

        for (auto& index : slot.indices) {
            index += startIndex;
        }

        // NOTE: concatenate two vectors
        result.vertices.insert(std::end(result.vertices), std::begin(slot.vertices), std::end(slot.vertices));
        result.indices.insert(std::end(result.indices), std::begin(slot.indices), std::end(slot.indices));
    }
    return std::make_tuple(std::move(result), nullptr);
}

} // namespace

std::tuple<SkinnedMesh, std::unique_ptr<Error>>
createSkinnedMesh(
    const std::vector<Matrix3x2>& bindPosesInGlobal,
    const SkinData& skinData,
    const TextureAtlas& textureAtlas,
    const Vector2& textureSize,
    std::string_view skinName)
{
    const auto* skin = skinData.findSkin(computeStringHash32(skinName));
    if (skin == nullptr) {
        auto err = errors::make(pomdog::format("cannot find skin '{}'", skinName));
        return std::make_tuple(SkinnedMesh{}, std::move(err));
    }

    auto [meshData, meshErr] = createVertices(*skin, textureAtlas, textureSize, bindPosesInGlobal);
    if (meshErr != nullptr) {
        auto wrapped = errors::wrap(std::move(meshErr), pomdog::format("failed to create skinned mesh of skin '{}'", skinName));
        return std::make_tuple(SkinnedMesh{}, std::move(wrapped));
    }
    return std::make_tuple(std::move(meshData), nullptr);
}

} // namespace pomdog::skeletal2d
