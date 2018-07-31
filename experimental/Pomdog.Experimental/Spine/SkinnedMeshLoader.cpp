// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "SkinnedMeshLoader.hpp"
#include "SkeletonDesc.hpp"
#include "Pomdog.Experimental/Skeletal2D/SkeletonPose.hpp"
#include "Pomdog.Experimental/Skeletal2D/SkinnedVertex.hpp"
#include "Pomdog.Experimental/Skeletal2D/SkinnedMesh.hpp"
#include "Pomdog/Experimental/TexturePacker/TextureAtlas.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include <algorithm>
#include <utility>
#include <tuple>

namespace Pomdog {
namespace Spine {
namespace {

struct SkinnedMeshSlot {
    std::vector<SkinnedVertex> Vertices;
    std::vector<std::uint16_t> Indices;
    std::uint16_t DrawOrder;
};

struct SkinnedMeshData {
    std::vector<SkinnedVertex> Vertices;
    std::vector<std::uint16_t> Indices;
    std::vector<SkinnedMeshPart> MeshParts;
};

using TexturePacker::TextureAtlas;
using TexturePacker::TextureAtlasRegion;

SkinnedMeshSlot CreateSkinnedMeshSlot(SlotDesc const& slotDesc,
    AttachmentDesc const& attachment, TextureRegion const& textureRegion,
    Vector2 const& textureSize, std::vector<Matrix3x2> const& bindPosesInGlobal)
{
    POMDOG_ASSERT(slotDesc.Joint);

    SkinnedMeshSlot slot;

    ///@todo Not implemented

    std::array<float, 4> weights = {{1, 0, 0, 0}};
    std::array<std::int32_t, 4> joints = {{*slotDesc.Joint, -1, -1, -1}};

    slot.Vertices = {
        {Vector4{0.0f, 0.0f, 0.0f, 1.0f}, weights, joints},
        {Vector4{0.0f, 1.0f, 0.0f, 0.0f}, weights, joints},
        {Vector4{1.0f, 1.0f, 1.0f, 0.0f}, weights, joints},
        {Vector4{1.0f, 0.0f, 1.0f, 1.0f}, weights, joints},
    };

    slot.Indices = {
        0, 1, 2,
        2, 3, 0
    };

    Vector2 origin;
    origin.X = static_cast<float>(textureRegion.Width/2 - textureRegion.XOffset)/textureRegion.Subrect.Width;
    origin.Y = static_cast<float>(textureRegion.Height/2 - textureRegion.YOffset)/textureRegion.Subrect.Height;

    Vector2 size(textureRegion.Subrect.Width, textureRegion.Subrect.Height);

    if (textureRegion.Rotate) {
        std::swap(size.X, size.Y);
        std::swap(origin.X, origin.Y);
    }

    Matrix3x2 scaling = Matrix3x2::CreateScale(attachment.Scale * size);
    Matrix3x2 rotate = Matrix3x2::CreateRotation(attachment.Rotation);
    if (textureRegion.Rotate) {
        rotate = Matrix3x2::CreateRotation(-Math::PiOver2<float>) * rotate;
    }
    Matrix3x2 translate = Matrix3x2::CreateTranslation(attachment.Translate);

    POMDOG_ASSERT(*slotDesc.Joint < bindPosesInGlobal.size());
    Matrix3x2 transform = scaling * rotate * translate * bindPosesInGlobal[*slotDesc.Joint];

    for (auto & vertex: slot.Vertices)
    {
        auto position = Vector2::Transform(Vector2(vertex.PositionTextureCoord.X, vertex.PositionTextureCoord.Y) - origin, transform);

        vertex.PositionTextureCoord.X = position.X;
        vertex.PositionTextureCoord.Y = position.Y;
    }

    for (auto & vertex: slot.Vertices)
    {
        auto position = Vector2(vertex.PositionTextureCoord.Z, vertex.PositionTextureCoord.W)
            * size
            + Vector2(textureRegion.Subrect.X, textureRegion.Subrect.Y);

        POMDOG_ASSERT(textureSize.X > 0);
        POMDOG_ASSERT(textureSize.Y > 0);
        position = position / textureSize;

        vertex.PositionTextureCoord.Z = position.X;
        vertex.PositionTextureCoord.W = position.Y;
    }

    return slot;
}

SkinnedMeshSlot CreateSkinnedMeshSlot(
    SkinnedMeshAttachmentDesc const& attachment, TextureRegion const& textureRegion,
    Vector2 const& textureSize, std::vector<Matrix3x2> const& bindPosesInGlobal)
{
    SkinnedMeshSlot meshSlot;
    for (auto & source: attachment.Vertices)
    {
        SkinnedVertex vertex;

        POMDOG_ASSERT(!source.Joints.empty());
        POMDOG_ASSERT(source.Joints.front());
        POMDOG_ASSERT(*source.Joints.front() < bindPosesInGlobal.size());
        auto position = Vector2::Transform(source.Position, bindPosesInGlobal[*source.Joints.front()]);

        Vector2 originInUV(textureRegion.Subrect.X, textureRegion.Subrect.Y);
        Vector2 sizeInUV(textureRegion.Subrect.Width, textureRegion.Subrect.Height);

        Vector2 textureCoordInUV = (textureRegion.Rotate
            ? Vector2{source.TextureCoordinate.Y * sizeInUV.Y, (1 - source.TextureCoordinate.X) * sizeInUV.X}
            : source.TextureCoordinate * sizeInUV);

        auto textureCoord = (originInUV + textureCoordInUV) / textureSize;

        vertex.PositionTextureCoord = {
            position.X, position.Y, textureCoord.X, textureCoord.Y
        };

        for (size_t i = 0; i < source.Joints.size(); ++i)
        {
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

SkinnedMeshData
CreateVertices(std::vector<SlotDesc> const& slots, SkinDesc const& skin,
    TextureAtlas const& textureAtlas, Vector2 const& textureSize,
    std::vector<Matrix3x2> const& bindPosesInGlobal)
{
    std::vector<SkinnedMeshSlot> meshSlots;
    meshSlots.reserve(slots.size());

    std::uint16_t drawOrder = 0;

    for (auto & slotDesc: slots)
    {
        POMDOG_ASSERT(drawOrder < std::numeric_limits<decltype(drawOrder)>::max());

        auto skinSlot = std::find_if(std::begin(skin.Slots), std::end(skin.Slots), [&slotDesc](SkinSlotDesc const& desc) {
            return desc.SlotName == slotDesc.Name;
        });

        POMDOG_ASSERT(skinSlot != std::end(skin.Slots));

        if (skinSlot == std::end(skin.Slots)) {
            ///@todo Not implemented
            // Error
            continue;
        }

        if (skinSlot->Attachments.empty() && !skinSlot->SkinnedMeshAttachments.empty())
        {
            auto attachment = std::find_if(std::begin(skinSlot->SkinnedMeshAttachments), std::end(skinSlot->SkinnedMeshAttachments),
                [&slotDesc](SkinnedMeshAttachmentDesc const& desc) {
                    return desc.Name == slotDesc.Attachement;
                });

            POMDOG_ASSERT(attachment != std::end(skinSlot->SkinnedMeshAttachments));

            auto textureAtlasRegion = std::find_if(std::begin(textureAtlas.regions), std::end(textureAtlas.regions),
                [&attachment](TextureAtlasRegion const& region) {
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
            [&slotDesc](AttachmentDesc const& desc) {
                return desc.Name == slotDesc.Attachement;
            });

        POMDOG_ASSERT(attachment != std::end(skinSlot->Attachments));

        if (attachment == std::end(skinSlot->Attachments)) {
            ///@todo Not implemented
            // Error
            continue;
        }

        auto textureAtlasRegion = std::find_if(std::begin(textureAtlas.regions), std::end(textureAtlas.regions),
            [&attachment](TextureAtlasRegion const& region) {
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
        [](SkinnedMeshSlot const& a, SkinnedMeshSlot const& b) {
            return a.DrawOrder < b.DrawOrder;
        });


    SkinnedMeshData result;
    result.MeshParts.reserve(meshSlots.size());

    std::uint16_t startIndex = 0;
    for (auto & slot: meshSlots)
    {
        {
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
        }
        {
            for (auto & index: slot.Indices) {
                index += startIndex;
            }

            POMDOG_ASSERT(startIndex + slot.Vertices.size() <= std::numeric_limits<decltype(startIndex)>::max());
            startIndex += slot.Vertices.size();

            // Note: concatenate two vectors
            result.Vertices.insert(std::end(result.Vertices), std::begin(slot.Vertices), std::end(slot.Vertices));
            result.Indices.insert(std::end(result.Indices), std::begin(slot.Indices), std::end(slot.Indices));
        }
    }
    return result;
}

} // unnamed namespace

SkinnedMesh CreateSkinnedMesh(
    GraphicsDevice & graphicsDevice,
    std::vector<Matrix3x2> const& bindPosesInGlobal,
    SkeletonDesc const& skeletonDesc,
    TexturePacker::TextureAtlas const& textureAtlas,
    Vector2 const& textureSize,
    std::string const& skinName)
{
    POMDOG_ASSERT(!skeletonDesc.Bones.empty());
    POMDOG_ASSERT(!skeletonDesc.Skins.empty());

    auto iter = std::find_if(std::begin(skeletonDesc.Skins), std::end(skeletonDesc.Skins),
        [&skinName](SkinDesc const& desc){ return desc.Name == skinName; });

    POMDOG_ASSERT(iter != std::end(skeletonDesc.Skins));

    if (iter == std::end(skeletonDesc.Skins)) {
        iter = std::begin(skeletonDesc.Skins);
    }

    POMDOG_ASSERT(iter != std::end(skeletonDesc.Skins));

    ///@todo Notimplemented
//    if (iter == std::end(skeletonDesc.Skins)) {
//        throw NotFound;
//    }

    auto meshData = CreateVertices(skeletonDesc.Slots, *iter, textureAtlas, textureSize, bindPosesInGlobal);

    SkinnedMesh skinnedMesh;
    skinnedMesh.VertexBuffer = std::make_shared<VertexBuffer>(graphicsDevice,
        meshData.Vertices.data(), meshData.Vertices.size(), sizeof(SkinnedVertex), BufferUsage::Immutable);
    skinnedMesh.IndexBuffer = std::make_shared<IndexBuffer>(
        graphicsDevice, IndexElementSize::SixteenBits,
        meshData.Indices.data(), meshData.Indices.size(), BufferUsage::Immutable);
    skinnedMesh.MeshParts = std::move(meshData.MeshParts);

    return skinnedMesh;
}

} // namespace Spine
} // namespace Pomdog
