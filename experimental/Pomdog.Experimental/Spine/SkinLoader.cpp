// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "SkinLoader.hpp"
#include "SkeletonDesc.hpp"
#include "Pomdog.Experimental/Skeletal2D/Skin.hpp"
#include "Pomdog/Utility/detail/CRC32.hpp"
#include <algorithm>

namespace Pomdog {
namespace Spine {
namespace {

std::vector<RigidSlot> CreateSlots(std::vector<SlotDesc> const& slotDescs,
    std::vector<SkinSlotDesc> const& skinSlotDescs,
    TexturePacker::TextureAtlas const& textureAtlas)
{
    using TexturePacker::TextureAtlas;
    using TexturePacker::TextureAtlasRegion;
    using Detail::CRC32;

    std::vector<RigidSlot> slots;
    slots.reserve(slotDescs.size());

    std::int16_t drawOrder = 0;

    for (auto & slotDesc: slotDescs)
    {
        auto iter = std::find_if(std::begin(skinSlotDescs), std::end(skinSlotDescs), [&slotDesc](SkinSlotDesc const& desc) {
            return desc.SlotName == slotDesc.Name;
        });

        POMDOG_ASSERT(iter != std::end(skinSlotDescs));

        if (iter == std::end(skinSlotDescs)) {
            ///@todo Not implemented
            // Error
            continue;
        }

        if (iter->Attachments.empty() && !iter->SkinnedMeshAttachments.empty())
        {
            ///@todo Not implemented

//            ///@note push dummy attachment data
//            RigidSlot slot;
//            slot.JointIndex = JointIndex(0);
//            slot.DrawOrder = drawOrder;
//            slot.Scale = {1, 1};
//            slot.Translate = {0, 0};
//            slot.Rotation = 0;
//            slot.TexturePage = 0;
//            slot.TextureRotate = false;
//            slot.Subrect = {0, 0, 1, 1};
//            slot.Origin.X = 0;
//            slot.Origin.Y = 0;
//
//            slots.push_back(std::move(slot));
//            ++drawOrder;
            continue;
        }

        POMDOG_ASSERT(!iter->Attachments.empty());

        if (iter->Attachments.empty()) {
            ///@todo Not implemented
            // Error
            continue;
        }

        auto attachment = std::find_if(std::begin(iter->Attachments), std::end(iter->Attachments), [&slotDesc](AttachmentDesc const& desc) {
            return desc.Name == slotDesc.Attachement;
        });

        POMDOG_ASSERT(attachment != std::end(iter->Attachments));

        if (attachment == std::end(iter->Attachments)) {
            ///@todo Not implemented
            // Error
            continue;
        }

        auto textureAtlasRegion = std::find_if(std::begin(textureAtlas.regions), std::end(textureAtlas.regions), [&attachment](TextureAtlasRegion const& region) {
            return region.Name == attachment->Name;
        });

        POMDOG_ASSERT(textureAtlasRegion != std::end(textureAtlas.regions));

        if (textureAtlasRegion == std::end(textureAtlas.regions)) {
            ///@todo Not implemented
            // Error
            continue;
        }

        RigidSlot slot;

        POMDOG_ASSERT(slotDesc.Joint);
        slot.JointIndex = slotDesc.Joint;
        slot.HashId = CRC32::ComputeCRC32(slotDesc.Name);

        slot.Color = Color::White;
        slot.DrawOrder = drawOrder;
        slot.Scale = attachment->Scale;
        slot.Translate = attachment->Translate;
        slot.Rotation = attachment->Rotation;

        slot.TexturePage = textureAtlasRegion->TexturePage;
        slot.TextureRotate = textureAtlasRegion->Region.Rotate;
        slot.Subrect = textureAtlasRegion->Region.Subrect;
        auto textureXOffset = textureAtlasRegion->Region.XOffset;
        auto textureYOffset = textureAtlasRegion->Region.YOffset;
        slot.Origin.X = static_cast<float>(textureAtlasRegion->Region.Width / 2 - textureXOffset)
            / textureAtlasRegion->Region.Subrect.Width;
        slot.Origin.Y = static_cast<float>(textureAtlasRegion->Region.Height / 2 - textureYOffset)
            / textureAtlasRegion->Region.Subrect.Height;

        if (textureAtlasRegion->Region.Rotate) {
            std::swap(slot.Subrect.Width, slot.Subrect.Height);
            std::swap(slot.Origin.X, slot.Origin.Y);
        }

        slots.push_back(std::move(slot));
        ++drawOrder;
    }

    return slots;
}

} // unnamed namespace

Skin CreateSkin(SkeletonDesc const& skeletonDesc,
    TexturePacker::TextureAtlas const& textureAtlas,
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

    Skin skin(CreateSlots(skeletonDesc.Slots, iter->Slots, textureAtlas));
    return skin;
}

} // namespace Spine
} // namespace Pomdog
