// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/spine/skinned_mesh_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/skeletal2d/skeleton_pose.h"
#include "pomdog/experimental/skeletal2d/skinned_mesh.h"
#include "pomdog/experimental/spine/skeleton_desc.h"
#include "pomdog/experimental/texture_packer/texture_atlas.h"
#include "pomdog/math/math.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <tuple>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::spine {
namespace {

using skeletal2d::SkinnedMeshPart;
using skeletal2d::SkinnedVertex;
using TexturePacker::TextureAtlas;
using TexturePacker::TextureAtlasRegion;

struct SkinnedMeshSlot final {
    std::vector<skeletal2d::SkinnedVertex> Vertices;
    std::vector<std::uint16_t> Indices;
    std::uint16_t DrawOrder;
};

SkinnedMeshSlot
CreateSkinnedMeshSlot(
    const SlotDesc& slotDesc,
    const AttachmentDesc& attachment,
    const TextureRegion& textureRegion,
    const Vector2& textureSize,
    const std::vector<Matrix3x2>& bindPosesInGlobal)
{
    POMDOG_ASSERT(slotDesc.Joint);

    SkinnedMeshSlot slot;

    ///@todo Not implemented

    std::array<float, 4> weights = {{1.0f, 0.0f, 0.0f, 0.0f}};
    std::array<std::int32_t, 4> joints = {{*slotDesc.Joint, -1, -1, -1}};

    slot.Vertices = {
        {Vector4{0.0f, 0.0f, 0.0f, 1.0f}, weights, joints},
        {Vector4{0.0f, 1.0f, 0.0f, 0.0f}, weights, joints},
        {Vector4{1.0f, 1.0f, 1.0f, 0.0f}, weights, joints},
        {Vector4{1.0f, 0.0f, 1.0f, 1.0f}, weights, joints},
    };

    slot.Indices = {0, 1, 2, 2, 3, 0};

    Vector2 origin;
    origin.X = static_cast<float>(textureRegion.Width / 2 - textureRegion.XOffset) / textureRegion.Subrect.Width;
    origin.Y = static_cast<float>(textureRegion.Height / 2 - textureRegion.YOffset) / textureRegion.Subrect.Height;

    auto size = Vector2{
        static_cast<float>(textureRegion.Subrect.Width),
        static_cast<float>(textureRegion.Subrect.Height),
    };

    if (textureRegion.Rotate) {
        std::swap(size.X, size.Y);
        std::swap(origin.X, origin.Y);
    }

    Matrix3x2 scaling = Matrix3x2::CreateScale(attachment.Scale * size);
    Matrix3x2 rotate = Matrix3x2::CreateRotation(attachment.Rotation);
    if (textureRegion.Rotate) {
        rotate = Matrix3x2::CreateRotation(-math::PiOver2<float>) * rotate;
    }
    Matrix3x2 translate = Matrix3x2::CreateTranslation(attachment.Translate);

    POMDOG_ASSERT(*slotDesc.Joint < bindPosesInGlobal.size());
    Matrix3x2 transform = scaling * rotate * translate * bindPosesInGlobal[*slotDesc.Joint];

    for (auto& vertex : slot.Vertices) {
        auto position = math::Transform(Vector2(vertex.PositionTextureCoord.X, vertex.PositionTextureCoord.Y) - origin, transform);

        vertex.PositionTextureCoord.X = position.X;
        vertex.PositionTextureCoord.Y = position.Y;
    }

    for (auto& vertex : slot.Vertices) {
        auto position = Vector2(vertex.PositionTextureCoord.Z, vertex.PositionTextureCoord.W)
            * size
            + Vector2{static_cast<float>(textureRegion.Subrect.X), static_cast<float>(textureRegion.Subrect.Y)};

        POMDOG_ASSERT(textureSize.X > 0);
        POMDOG_ASSERT(textureSize.Y > 0);
        position = position / textureSize;

        vertex.PositionTextureCoord.Z = position.X;
        vertex.PositionTextureCoord.W = position.Y;
    }

    return slot;
}

SkinnedMeshSlot
CreateSkinnedMeshSlot(
    const SkinnedMeshAttachmentDesc& attachment,
    const TextureRegion& textureRegion,
    const Vector2& textureSize,
    const std::vector<Matrix3x2>& bindPosesInGlobal)
{
    SkinnedMeshSlot meshSlot;
    for (auto& source : attachment.Vertices) {
        SkinnedVertex vertex;

        POMDOG_ASSERT(!source.Joints.empty());
        POMDOG_ASSERT(source.Joints.front());
        POMDOG_ASSERT(*source.Joints.front() < bindPosesInGlobal.size());
        auto position = math::Transform(source.Position, bindPosesInGlobal[*source.Joints.front()]);

        auto originInUV = Vector2{
            static_cast<float>(textureRegion.Subrect.X),
            static_cast<float>(textureRegion.Subrect.Y),
        };
        auto sizeInUV = Vector2{
            static_cast<float>(textureRegion.Subrect.Width),
            static_cast<float>(textureRegion.Subrect.Height),
        };

        Vector2 textureCoordInUV = (textureRegion.Rotate
            ? Vector2{source.TextureCoordinate.Y * sizeInUV.Y, (1 - source.TextureCoordinate.X) * sizeInUV.X}
            : source.TextureCoordinate * sizeInUV);

        auto textureCoord = (originInUV + textureCoordInUV) / textureSize;

        vertex.PositionTextureCoord = {
            position.X, position.Y, textureCoord.X, textureCoord.Y
        };

        for (size_t i = 0; i < source.Joints.size(); ++i) {
            if (source.Joints[i]) {
                vertex.Joints[i] = *source.Joints[i];
            }
            else {
                vertex.Joints[i] = -1;
            }
        }

        vertex.Weights[0] = source.Weights[0];
        vertex.Weights[1] = source.Weights[1];
        vertex.Weights[2] = source.Weights[2];
        vertex.Weights[3] = 1.0f - (source.Weights[0] + source.Weights[1] + source.Weights[2]);

        meshSlot.Vertices.push_back(std::move(vertex));
    }
    meshSlot.Indices = attachment.Indices;
    return meshSlot;
}

skeletal2d::SkinnedMesh
CreateVertices(
    const std::vector<SlotDesc>& slots,
    const SkinDesc& skin,
    const TextureAtlas& textureAtlas,
    const Vector2& textureSize,
    const std::vector<Matrix3x2>& bindPosesInGlobal)
{
    std::vector<SkinnedMeshSlot> meshSlots;
    meshSlots.reserve(slots.size());

    std::uint16_t drawOrder = 0;

    for (auto& slotDesc : slots) {
        POMDOG_ASSERT(drawOrder < std::numeric_limits<decltype(drawOrder)>::max());

        auto skinSlot = std::find_if(std::begin(skin.Slots), std::end(skin.Slots), [&slotDesc](const SkinSlotDesc& desc) {
            return desc.SlotName == slotDesc.Name;
        });

        POMDOG_ASSERT(skinSlot != std::end(skin.Slots));

        if (skinSlot == std::end(skin.Slots)) {
            ///@todo Not implemented
            // Error
            continue;
        }

        if (skinSlot->Attachments.empty() && !skinSlot->SkinnedMeshAttachments.empty()) {
            auto attachment = std::find_if(std::begin(skinSlot->SkinnedMeshAttachments), std::end(skinSlot->SkinnedMeshAttachments),
                [&slotDesc](const SkinnedMeshAttachmentDesc& desc) {
                    return desc.Name == slotDesc.Attachement;
                });

            POMDOG_ASSERT(attachment != std::end(skinSlot->SkinnedMeshAttachments));

            auto textureAtlasRegion = std::find_if(std::begin(textureAtlas.regions), std::end(textureAtlas.regions),
                [&attachment](const TextureAtlasRegion& region) {
                    return region.Name == attachment->Name;
                });

            POMDOG_ASSERT(textureAtlasRegion != std::end(textureAtlas.regions));
            SkinnedMeshSlot meshSlot = CreateSkinnedMeshSlot(*attachment, textureAtlasRegion->Region, textureSize, bindPosesInGlobal);
            meshSlot.DrawOrder = drawOrder;
            meshSlots.push_back(std::move(meshSlot));
            ++drawOrder;
            continue;
        }

        POMDOG_ASSERT(!skinSlot->Attachments.empty());

        if (skinSlot->Attachments.empty()) {
            ///@todo Not implemented
            // Error
            continue;
        }

        auto attachment = std::find_if(std::begin(skinSlot->Attachments), std::end(skinSlot->Attachments),
            [&slotDesc](const AttachmentDesc& desc) {
                return desc.Name == slotDesc.Attachement;
            });

        POMDOG_ASSERT(attachment != std::end(skinSlot->Attachments));

        if (attachment == std::end(skinSlot->Attachments)) {
            ///@todo Not implemented
            // Error
            continue;
        }

        auto textureAtlasRegion = std::find_if(std::begin(textureAtlas.regions), std::end(textureAtlas.regions),
            [&attachment](const TextureAtlasRegion& region) {
                return region.Name == attachment->Name;
            });

        POMDOG_ASSERT(textureAtlasRegion != std::end(textureAtlas.regions));

        if (textureAtlasRegion == std::end(textureAtlas.regions)) {
            ///@todo Not implemented
            // Error
            continue;
        }

        POMDOG_ASSERT(slotDesc.Joint);

        auto meshSlot = CreateSkinnedMeshSlot(slotDesc, *attachment,
            textureAtlasRegion->Region, textureSize, bindPosesInGlobal);
        meshSlot.DrawOrder = drawOrder;
        meshSlots.push_back(std::move(meshSlot));
        ++drawOrder;
    }

    std::sort(std::begin(meshSlots), std::end(meshSlots),
        [](const SkinnedMeshSlot& a, const SkinnedMeshSlot& b) {
            return a.DrawOrder < b.DrawOrder;
        });

    skeletal2d::SkinnedMesh result;
    result.MeshParts.reserve(meshSlots.size());

    std::uint16_t startIndex = 0;
    for (auto& slot : meshSlots) {
        SkinnedMeshPart meshPart;

        POMDOG_ASSERT(result.Vertices.size() < std::numeric_limits<std::uint16_t>::max());
        meshPart.VertexOffset = static_cast<std::uint16_t>(result.Vertices.size());

        POMDOG_ASSERT(slot.Vertices.size() < std::numeric_limits<std::uint16_t>::max());
        meshPart.VertexCount = static_cast<std::uint16_t>(slot.Vertices.size());

        POMDOG_ASSERT(result.Indices.size() < std::numeric_limits<std::uint16_t>::max());
        meshPart.StartIndex = static_cast<std::uint16_t>(result.Indices.size());

        POMDOG_ASSERT(slot.Indices.size() % 3 == 0);
        meshPart.PrimitiveCount = static_cast<std::uint16_t>(slot.Indices.size()) / 3;
        result.MeshParts.push_back(std::move(meshPart));

        for (auto& index : slot.Indices) {
            index += startIndex;
        }

        POMDOG_ASSERT(startIndex + slot.Vertices.size() <= std::numeric_limits<decltype(startIndex)>::max());
        startIndex += static_cast<decltype(startIndex)>(slot.Vertices.size());

        // NOTE: concatenate two vectors
        result.Vertices.insert(std::end(result.Vertices), std::begin(slot.Vertices), std::end(slot.Vertices));
        result.Indices.insert(std::end(result.Indices), std::begin(slot.Indices), std::end(slot.Indices));
    }
    return result;
}

} // namespace

std::tuple<skeletal2d::SkinnedMesh, std::unique_ptr<Error>>
CreateSkinnedMesh(
    const std::vector<Matrix3x2>& bindPosesInGlobal,
    const SkeletonDesc& skeletonDesc,
    const TexturePacker::TextureAtlas& textureAtlas,
    const Vector2& textureSize,
    const std::string& skinName)
{
    POMDOG_ASSERT(!skeletonDesc.Bones.empty());
    POMDOG_ASSERT(!skeletonDesc.Skins.empty());

    auto iter = std::find_if(std::begin(skeletonDesc.Skins), std::end(skeletonDesc.Skins),
        [&](const SkinDesc& desc) { return desc.Name == skinName; });

    if (iter == std::end(skeletonDesc.Skins)) {
        auto err = errors::New("Skin not found '" + skinName + "'");
        return std::make_tuple(skeletal2d::SkinnedMesh{}, std::move(err));
    }

    POMDOG_ASSERT(iter != std::end(skeletonDesc.Skins));

    auto meshData = CreateVertices(skeletonDesc.Slots, *iter, textureAtlas, textureSize, bindPosesInGlobal);
    return std::make_tuple(std::move(meshData), nullptr);
}

} // namespace pomdog::spine
