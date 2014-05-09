//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "SkinLoader.hpp"
#include "Skin.hpp"

namespace Pomdog {
namespace Details {
namespace Skeletal2D {
namespace {
//-----------------------------------------------------------------------
static std::vector<RigidSlot> CreateSlots(std::vector<Skeletal2D::SlotDesc> const& slotDescs,
	std::vector<Skeletal2D::SkinSlotDesc> const& skinSlotDescs,
	std::vector<Skeletal2D::BoneDesc> const& bones,
	Details::TexturePacker::TextureAtlas const& textureAtlas)
{
	using Skeletal2D::SkinSlotDesc;
	using Skeletal2D::AttachmentDesc;
	using Skeletal2D::BoneDesc;
	using Details::TexturePacker::TextureAtlas;
	using Details::TexturePacker::TextureAtlasRegion;

	std::vector<RigidSlot> slots;
	slots.reserve(slotDescs.size());
	
	std::uint16_t drawOrder = 0;
	
	for (auto & slotDesc: slotDescs)
	{
		auto iter = std::find_if(std::begin(skinSlotDescs), std::end(skinSlotDescs), [&slotDesc](SkinSlotDesc const& desc) {
			return desc.Name == slotDesc.Name;
		});
		
		POMDOG_ASSERT(iter != std::end(skinSlotDescs));
		
		if (iter == std::end(skinSlotDescs)) {
			///@todo Not implement
			// Error
			continue;
		}

		POMDOG_ASSERT(!iter->Attachments.empty());

		if (iter->Attachments.empty()) {
			///@todo Not implement
			// Error
			continue;
		}

		auto attachment = std::find_if(std::begin(iter->Attachments), std::end(iter->Attachments), [&slotDesc](AttachmentDesc const& desc) {
			return desc.Name == slotDesc.Attachement;
		});

		POMDOG_ASSERT(attachment != std::end(iter->Attachments));

		if (attachment == std::end(iter->Attachments)) {
			///@todo Not implement
			// Error
			continue;
		}
		
		auto textureAtlasRegion = std::find_if(std::begin(textureAtlas.regions), std::end(textureAtlas.regions), [&attachment](TextureAtlasRegion const& region) {
			return region.Name == attachment->Name;
		});
		
		POMDOG_ASSERT(textureAtlasRegion != std::end(textureAtlas.regions));

		if (textureAtlasRegion == std::end(textureAtlas.regions)) {
			///@todo Not implement
			// Error
			continue;
		}
		
		auto boneDesc = std::find_if(std::begin(bones), std::end(bones), [&slotDesc](BoneDesc const& desc) {
			return desc.Name == slotDesc.Bone;
		});
		
		POMDOG_ASSERT(boneDesc != std::end(bones));

		if (boneDesc == std::end(bones)) {
			///@todo Not implement
			// Error
			continue;
		}

		RigidSlot slot;
		slot.Color = Color::White;
		slot.DrawOrder = drawOrder;
		slot.Scale = attachment->Scale;
		slot.Translate = attachment->Translate;
		slot.Rotation = attachment->Rotation;
		
		slot.TexturePage = textureAtlasRegion->TexturePage;
		slot.TextureRotate = textureAtlasRegion->Rotate;
		slot.Subrect = textureAtlasRegion->Subrect;
		auto textureXOffset = textureAtlasRegion->XOffset;
		auto textureYOffset = textureAtlasRegion->YOffset;
		slot.Origin.X = static_cast<float>(textureAtlasRegion->OriginalWidth/2 - textureXOffset)/textureAtlasRegion->Subrect.Width;
		slot.Origin.Y = static_cast<float>(textureAtlasRegion->OriginalHeight/2 - textureYOffset)/textureAtlasRegion->Subrect.Height;
		
		if (textureAtlasRegion->Rotate) {
			std::swap(slot.Subrect.Width, slot.Subrect.Height);
			std::swap(slot.Origin.X, slot.Origin.Y);
		}
		
		slot.JointIndex = std::distance(std::begin(bones), boneDesc);

		slots.push_back(std::move(slot));
		++drawOrder;
	}
	
	return std::move(slots);
}

}// unnamed namespace
//-----------------------------------------------------------------------
Skin CreateSkin(Skeletal2D::SkeletonDesc const& skeletonDesc,
	Details::TexturePacker::TextureAtlas const& textureAtlas,
	std::string const& skinName)
{
	POMDOG_ASSERT(!skeletonDesc.Bones.empty());
	POMDOG_ASSERT(!skeletonDesc.Skins.empty());

	auto iter = std::find_if(std::begin(skeletonDesc.Skins), std::end(skeletonDesc.Skins),
		[&skinName](Skeletal2D::SkinDesc const& desc){ return desc.Name == skinName; });

	POMDOG_ASSERT(iter != std::end(skeletonDesc.Skins));

	if (iter == std::end(skeletonDesc.Skins)) {
		iter = std::begin(skeletonDesc.Skins);
	}

	POMDOG_ASSERT(iter != std::end(skeletonDesc.Skins));
	
	Skin skin(CreateSlots(skeletonDesc.Slots, iter->Slots, skeletonDesc.Bones, textureAtlas));
	return std::move(skin);
}

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog
