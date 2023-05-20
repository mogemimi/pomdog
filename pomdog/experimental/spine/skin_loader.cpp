// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/spine/skin_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/skeletal2d/skin.h"
#include "pomdog/experimental/spine/skeleton_desc.h"
#include "pomdog/utility/crc32.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::spine {
namespace {

using skeletal2d::RigidSlot;
using TexturePacker::TextureAtlas;
using TexturePacker::TextureAtlasRegion;

std::vector<RigidSlot>
CreateSlots(
    const std::vector<SlotDesc>& slotDescs,
    const std::vector<SkinSlotDesc>& skinSlotDescs,
    const TexturePacker::TextureAtlas& textureAtlas)
{
    std::vector<RigidSlot> slots;
    slots.reserve(slotDescs.size());

    std::int16_t drawOrder = 0;

    for (auto& slotDesc : slotDescs) {
        auto iter = std::find_if(std::begin(skinSlotDescs), std::end(skinSlotDescs), [&slotDesc](const SkinSlotDesc& desc) {
            return desc.SlotName == slotDesc.Name;
        });

        POMDOG_ASSERT(iter != std::end(skinSlotDescs));

        if (iter == std::end(skinSlotDescs)) {
            // TODO: Not implemented
            // FIXME: Add error handling
            continue;
        }

        if (iter->Attachments.empty() && !iter->SkinnedMeshAttachments.empty()) {
            // TODO: Not implemented

            // // NOTE: push dummy attachment data
            // RigidSlot slot;
            // slot.JointIndex = JointIndex(0);
            // slot.DrawOrder = drawOrder;
            // slot.Scale = {1, 1};
            // slot.Translate = {0, 0};
            // slot.Rotation = 0;
            // slot.TexturePage = 0;
            // slot.TextureRotate = false;
            // slot.Subrect = {0, 0, 1, 1};
            // slot.Origin.X = 0;
            // slot.Origin.Y = 0;
            //
            // slots.push_back(std::move(slot));
            // ++drawOrder;
            continue;
        }

        POMDOG_ASSERT(!iter->Attachments.empty());

        if (iter->Attachments.empty()) {
            // TODO: Not implemented
            // FIXME: Add error handling
            continue;
        }

        auto attachment = std::find_if(std::begin(iter->Attachments), std::end(iter->Attachments), [&slotDesc](const AttachmentDesc& desc) {
            return desc.Name == slotDesc.Attachement;
        });

        POMDOG_ASSERT(attachment != std::end(iter->Attachments));

        if (attachment == std::end(iter->Attachments)) {
            // TODO: Not implemented
            // FIXME: Add error handling
            continue;
        }

        auto textureAtlasRegion = std::find_if(std::begin(textureAtlas.regions), std::end(textureAtlas.regions), [&attachment](const TextureAtlasRegion& region) {
            return region.Name == attachment->Name;
        });

        POMDOG_ASSERT(textureAtlasRegion != std::end(textureAtlas.regions));

        if (textureAtlasRegion == std::end(textureAtlas.regions)) {
            // TODO: Not implemented
            // FIXME: Add error handling
            continue;
        }

        RigidSlot slot;

        POMDOG_ASSERT(slotDesc.Joint);
        slot.JointIndex = slotDesc.Joint;
        slot.HashID = detail::crc32::ComputeCRC32(slotDesc.Name);

        slot.Color = Color::White();
        slot.DrawOrder = drawOrder;
        slot.Scale = attachment->Scale;
        slot.Translate = attachment->Translate;
        slot.Rotation = attachment->Rotation;

        slot.TexturePage = textureAtlasRegion->TexturePage;
        slot.TextureRotate = textureAtlasRegion->Region.rotate;
        slot.Subrect = textureAtlasRegion->Region.subrect;
        auto textureXOffset = textureAtlasRegion->Region.xOffset;
        auto textureYOffset = textureAtlasRegion->Region.yOffset;
        slot.Origin.x = static_cast<float>(textureAtlasRegion->Region.width / 2 - textureXOffset) / textureAtlasRegion->Region.subrect.width;
        slot.Origin.y = static_cast<float>(textureAtlasRegion->Region.height / 2 - textureYOffset) / textureAtlasRegion->Region.subrect.height;

        if (textureAtlasRegion->Region.rotate) {
            std::swap(slot.Subrect.width, slot.Subrect.height);
            std::swap(slot.Origin.x, slot.Origin.y);
        }

        slots.push_back(std::move(slot));
        ++drawOrder;
    }

    return slots;
}

} // namespace

std::shared_ptr<skeletal2d::Skin>
CreateSkin(
    const SkeletonDesc& skeletonDesc,
    const TexturePacker::TextureAtlas& textureAtlas,
    const std::string& skinName)
{
    POMDOG_ASSERT(!skeletonDesc.Bones.empty());
    POMDOG_ASSERT(!skeletonDesc.Skins.empty());

    auto iter = std::find_if(std::begin(skeletonDesc.Skins), std::end(skeletonDesc.Skins),
        [&skinName](const SkinDesc& desc) { return desc.Name == skinName; });

    POMDOG_ASSERT(iter != std::end(skeletonDesc.Skins));

    if (iter == std::end(skeletonDesc.Skins)) {
        iter = std::begin(skeletonDesc.Skins);
    }

    POMDOG_ASSERT(iter != std::end(skeletonDesc.Skins));

    // TODO: Not implemented
    // if (iter == std::end(skeletonDesc.Skins)) {
    //     throw NotFound;
    // }

    auto slots = CreateSlots(skeletonDesc.Slots, iter->Slots, textureAtlas);
    auto skin = std::make_shared<skeletal2d::Skin>(std::move(slots));
    return skin;
}

} // namespace pomdog::spine
