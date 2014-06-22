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
#include "SkeletonPose.hpp"
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
	AttachmentDesc const& attachment, TextureRegion const& textureRegion,
	Vector2 const& textureSize, SkeletonPose const& bindPose)
{
	POMDOG_ASSERT(slotDesc.Joint);

	SkinnedMeshSlot slot;

	///@todo Not implemented

	std::array<float, 4> weights = {1, 0, 0, 0};
	std::array<std::int32_t, 4> joints = {*slotDesc.Joint, -1, -1, -1};

	slot.vertices = {
		{Vector4{0.0f, 0.0f, 0.0f, 1.0f}, weights, joints},
		{Vector4{0.0f, 1.0f, 0.0f, 0.0f}, weights, joints},
		{Vector4{1.0f, 1.0f, 1.0f, 0.0f}, weights, joints},
		{Vector4{1.0f, 0.0f, 1.0f, 1.0f}, weights, joints},
	};

	slot.indices = {
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
		rotate = Matrix3x2::CreateRotation(-MathConstants<float>::PiOver2()) * rotate;
	}
	Matrix3x2 translate = Matrix3x2::CreateTranslation(attachment.Translate);
	
	POMDOG_ASSERT(*slotDesc.Joint < bindPose.GlobalPose.size());
	Matrix3x2 transform = scaling * rotate * translate * bindPose.GlobalPose[*slotDesc.Joint];

	for (auto & vertex: slot.vertices)
	{
		auto position = Vector2::Transform(Vector2(vertex.PositionTextureCoord.X, vertex.PositionTextureCoord.Y) - origin, transform);
		
		vertex.PositionTextureCoord.X = position.X;
		vertex.PositionTextureCoord.Y = position.Y;
	}

	for (auto & vertex: slot.vertices)
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

	return std::move(slot);
}
//-----------------------------------------------------------------------
static std::tuple<std::vector<SkinnedVertex>, std::vector<std::uint16_t>>
CreateVertices(std::vector<SlotDesc> const& slots, SkinDesc const& skin,
		TextureAtlas const& textureAtlas, Vector2 const& textureSize, SkeletonPose const& bindPose)
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
			///@todo Not implemented

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
		
			auto & textureRegion = textureAtlasRegion->Region;
		
			SkinnedMeshSlot meshSlot;
			for (auto & source: attachment->Vertices)
			{
				SkinnedVertex vertex;
				
				POMDOG_ASSERT(!source.Joints.empty());
				POMDOG_ASSERT(source.Joints.front());
				POMDOG_ASSERT(*source.Joints.front() < bindPose.GlobalPose.size());
				auto position = Vector2::Transform(source.Position, bindPose.GlobalPose[*source.Joints.front()]);
				
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
				
				meshSlot.vertices.push_back(std::move(vertex));
			}
			meshSlot.indices = attachment->Indices;
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

		auto meshSlot = CreateSkinnedMeshSlot(slotDesc, *attachment, textureAtlasRegion->Region, textureSize, bindPose);
		meshSlot.DrawOrder = drawOrder;
		meshSlots.push_back(std::move(meshSlot));
		++drawOrder;
	}
	
	std::sort(std::begin(meshSlots), std::end(meshSlots),
		[](SkinnedMeshSlot const& a, SkinnedMeshSlot const& b) {
			return a.DrawOrder < b.DrawOrder;
		});

	std::vector<SkinnedVertex> vertices;
	std::vector<std::uint16_t> indices;
	
	std::uint16_t startIndex = 0;
	for (auto & slot: meshSlots)
	{
		for (auto & index: slot.indices) {
			index += startIndex;
		}
		
		POMDOG_ASSERT(startIndex + slot.vertices.size() <= std::numeric_limits<decltype(startIndex)>::max());
		startIndex += slot.vertices.size();

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
	SkeletonPose const& bindPose,
	SkeletonDesc const& skeletonDesc,
	Details::TexturePacker::TextureAtlas const& textureAtlas,
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
//	if (iter == std::end(skeletonDesc.Skins)) {
//		throw NotFound;
//	}
	
	POMDOG_ASSERT(graphicsDevice);
	
	auto primitives = CreateVertices(skeletonDesc.Slots, *iter, textureAtlas, textureSize, bindPose);
	auto vertices = std::move(std::get<0>(primitives));
	auto indices = std::move(std::get<1>(primitives));
	
	using SkinnedMeshVertex = CustomVertex<Vector4, Vector4, std::int32_t[4]>;
	static_assert(std::is_same<decltype(SkinnedVertex::PositionTextureCoord), Vector4>::value, "");
	static_assert(std::is_same<decltype(SkinnedVertex::Weights), std::array<float, 4>>::value, "");
	static_assert(std::is_same<decltype(SkinnedVertex::Joints), std::array<std::int32_t, 4>>::value, "");

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
