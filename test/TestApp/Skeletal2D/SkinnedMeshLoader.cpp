//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "SkinnedMeshLoader.hpp"
#include <algorithm>
#include <utility>
#include <tuple>
#include "SkeletonDesc.hpp"
#include "SkinnedVertex.hpp"
#include "SkinnedMesh.hpp"
#include "TextureAtlas.hpp"

namespace Pomdog {
namespace Details {
namespace Skeletal2D {
namespace {

struct SkinnedMeshSlot {
	std::vector<SkinnedVertex> vertices;
	std::vector<std::uint16_t> indices;
	std::uint16_t DrawOrder;
};

using Details::TexturePacker::TextureAtlas;
using Details::TexturePacker::TextureAtlasRegion;
//-----------------------------------------------------------------------
static SkinnedMeshSlot CreateSkinnedMeshSlot(SlotDesc const& slotDesc,
	SkinSlotDesc const& skinSlot, TextureAtlasRegion const& textureAtlasRegion)
{
	SkinnedMeshSlot slot;

	///@todo Not implemented

	std::array<float, 3> weights = {1, 0, 0};
	std::array<std::uint8_t, 4> joints = {*slotDesc.Joint, 0, 0, 0};

	slot.vertices = {
		{Vector2{0.0f, 0.0f}, Vector2{0.0f, 1.0f}, weights, joints},
		{Vector2{0.0f, 1.0f}, Vector2{0.0f, 0.0f}, weights, joints},
		{Vector2{1.0f, 1.0f}, Vector2{1.0f, 0.0f}, weights, joints},
		{Vector2{1.0f, 0.0f}, Vector2{1.0f, 1.0f}, weights, joints},
	};

	slot.indices = {
		0, 1, 2,
		2, 3, 0
	};

//	slot.Scale = attachment->Scale;
//	slot.Translate = attachment->Translate;
//	slot.Rotation = attachment->Rotation;
//	
//	slot.TexturePage = textureAtlasRegion.TexturePage;
//	slot.TextureRotate = textureAtlasRegion.Rotate;
//	slot.Subrect = textureAtlasRegion.Subrect;
//
//	slot.Origin.X = static_cast<float>(textureAtlasRegion.OriginalWidth/2 - textureAtlasRegion.XOffset)/textureAtlasRegion.Subrect.Width;
//	slot.Origin.Y = static_cast<float>(textureAtlasRegion.OriginalHeight/2 - textureAtlasRegion.YOffset)/textureAtlasRegion.Subrect.Height;
//
//	if (textureAtlasRegion.Rotate) {
//		std::swap(slot.Subrect.Width, slot.Subrect.Height);
//		std::swap(slot.Origin.X, slot.Origin.Y);
//	}

	return std::move(slot);
}
//-----------------------------------------------------------------------
static std::tuple<std::vector<SkinnedVertex>, std::vector<std::uint16_t>>
	CreateVertices(std::vector<SlotDesc> const& slots, SkinDesc const& skin, TextureAtlas const& textureAtlas)
{
	std::vector<SkinnedMeshSlot> meshSlots;
	meshSlots.reserve(slots.size());

	std::uint16_t drawOrder = 0;

	for (auto & slotDesc: slots)
	{
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
			///@todo Not implemented
			
			///@note push dummy attachment data
//			RigidSlot slot;
//			slot.JointIndex = JointIndex(0);
//			slot.DrawOrder = drawOrder;
//			slot.Scale = {1, 1};
//			slot.Translate = {0, 0};
//			slot.Rotation = 0;
//			slot.TexturePage = 0;
//			slot.TextureRotate = false;
//			slot.Subrect = {0, 0, 1, 1};
//			slot.Origin.X = 0;
//			slot.Origin.Y = 0;
//
//			slots.push_back(std::move(slot));
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

		auto meshSlot = CreateSkinnedMeshSlot(slotDesc, *skinSlot, *textureAtlasRegion);
		meshSlot.DrawOrder = drawOrder;
		meshSlots.push_back(std::move(meshSlot));
		++drawOrder;
	}
	
	std::sort(std::begin(meshSlots), std::end(meshSlots),
		[](SkinnedMeshSlot const& a, SkinnedMeshSlot const& b) {
			return a.DrawOrder > b.DrawOrder;
		});

	std::vector<SkinnedVertex> vertices;
	std::vector<std::uint16_t> indices;
	
	for (auto & slot: meshSlots)
	{
		// Note: concatenate two vectors
		vertices.insert(std::end(vertices), std::begin(slot.vertices), std::end(slot.vertices));
		indices.insert(std::end(indices), std::begin(slot.indices), std::end(slot.indices));
	}
	return {std::move(vertices), std::move(indices)};
}

}// unnamed namespace
//-----------------------------------------------------------------------
SkinnedMesh CreateSkinnedMesh(
	std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	SkeletonDesc const& skeletonDesc,
	Details::TexturePacker::TextureAtlas const& textureAtlas,
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
//	if (iter == std::end(skeletonDesc.Skins)) {
//		throw NotFound;
//	}
	
	POMDOG_ASSERT(graphicsDevice);
	
	auto primitives = CreateVertices(skeletonDesc.Slots, *iter, textureAtlas);
	auto vertices = std::move(std::get<0>(primitives));
	auto indices = std::move(std::get<1>(primitives));
	
	using SkinnedMeshVertex = CustomVertex<Vector2, Vector2, Vector3, std::uint8_t[4]>;
	
	SkinnedMesh skinnedMesh;
	skinnedMesh.VertexBuffer = std::make_shared<VertexBuffer>(
		graphicsDevice, SkinnedMeshVertex::Declaration(),
		vertices.data(), vertices.size(), BufferUsage::Immutable);
	skinnedMesh.IndexBuffer = std::make_shared<IndexBuffer>(
		graphicsDevice, IndexElementSize::SixteenBits,
		indices.data(), indices.size(), BufferUsage::Immutable);

	return std::move(skinnedMesh);
}

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog
