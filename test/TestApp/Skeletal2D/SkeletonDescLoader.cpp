//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "SkeletonDescLoader.hpp"
#include <utility>
#include <fstream>
#include <vector>
#include <algorithm>
#include <rapidjson/document.h>

namespace Pomdog {
namespace Details {
namespace {

using Skeletal2D::AttachmentDesc;
using Skeletal2D::BoneDesc;
using Skeletal2D::SlotDesc;
using Skeletal2D::SkeletonDesc;
using Skeletal2D::SkinDesc;
using Skeletal2D::SkinSlotDesc;
using Skeletal2D::SkinnedMeshVertexDesc;
using Skeletal2D::SkinnedMeshAttachmentDesc;
using Skeletal2D::AnimationSamplePointTranslate;
using Skeletal2D::AnimationSamplePointRotate;
using Skeletal2D::AnimationSamplePointScale;
using Skeletal2D::AnimationSamplePointAttachment;
using Skeletal2D::BoneAnimationTrackDesc;
using Skeletal2D::SlotAnimationTrackDesc;
using Skeletal2D::AnimationClipDesc;
using Skeletal2D::KeyframeCurve;

static std::vector<char> ReadBinaryFile(std::string const& filename)
{
	std::ifstream stream(filename, std::ios::in | std::ios::binary);
	
	stream.seekg(0, stream.end);
	auto const length = stream.tellg();
	stream.seekg(0, stream.beg);
	
	std::vector<char> result(length, 0);
	stream.read(result.data(), result.size());
	return std::move(result);
}
//-----------------------------------------------------------------------
static void ReadJsonMember(rapidjson::Value const& object, char const* memberName, std::string & output)
{
	if (!object.HasMember(memberName)) {
		return;
	}
	
	auto & memberObject = object[memberName];
	if (memberObject.IsString()) {
		output.assign(memberObject.GetString(), memberObject.GetStringLength());
	}
}
//-----------------------------------------------------------------------
static void ReadJsonMember(rapidjson::Value const& object, char const* memberName, float & output)
{
	if (!object.HasMember(memberName)) {
		return;
	}
	
	auto & memberObject = object[memberName];
	if (memberObject.IsNumber()) {
		output = memberObject.GetDouble();
	}
}
//-----------------------------------------------------------------------
static void ReadJsonMember(rapidjson::Value const& object, char const* memberName, Radian<float> & output)
{
	if (!object.HasMember(memberName)) {
		return;
	}
	
	auto & memberObject = object[memberName];
	if (memberObject.IsNumber()) {
		Degree<double> degreeAngle = memberObject.GetDouble();
		while (degreeAngle > 180) {
			degreeAngle -= 360;
		}
		while (degreeAngle < -180) {
			degreeAngle += 360;
		}
		output = MathHelper::ToRadians(degreeAngle).value;
	}
}
//-----------------------------------------------------------------------
static void ReadJsonMember(rapidjson::Value const& object, char const* memberName, Degree<double> & output)
{
	if (!object.HasMember(memberName)) {
		return;
	}
	
	auto & memberObject = object[memberName];
	if (memberObject.IsNumber()) {
		output.value = memberObject.GetDouble();
	}
}
//-----------------------------------------------------------------------
template <typename T, T Denom>
static void ReadJsonMember(rapidjson::Value const& object, char const* memberName, Skeletal2D::CompressedFloat<T, Denom> & output)
{
	if (!object.HasMember(memberName)) {
		return;
	}
	
	auto & memberObject = object[memberName];
	if (memberObject.IsNumber()) {
		output = memberObject.GetDouble();
	}
}
//-----------------------------------------------------------------------
static void ReadJsonMember(rapidjson::Value const& object, char const* memberName, KeyframeCurve & output)
{
	output = KeyframeCurve::Liener;
	if (!object.HasMember(memberName)) {
		return;
	}
	
	auto & memberObject = object[memberName];
	if (memberObject.IsString()) {
		if (memberObject.GetString() != std::string("linear")) {
			output = KeyframeCurve::Stepped;
		}
	}
	else if (memberObject.IsArray()) {
		///@todo Not implemented
		output = KeyframeCurve::Bezier;
	}
}
//-----------------------------------------------------------------------
static void ReadJsonMember(rapidjson::Value const& object, char const* memberName, std::uint16_t & output)
{
	if (!object.HasMember(memberName)) {
		return;
	}
	
	auto & memberObject = object[memberName];
	if (memberObject.IsUint()) {
		output = memberObject.GetUint();
	}
}
//-----------------------------------------------------------------------
static void ReadJsonMember(rapidjson::Value const& object, char const* memberName, Color & output)
{
	if (!object.HasMember(memberName)) {
		return;
	}
	
	auto & memberObject = object[memberName];
	if (memberObject.IsString()) {
		auto hexColor = std::stoul(memberObject.GetString(), 0, 16);
		output.R(hexColor >> 24);
		output.G(hexColor >> 16);
		output.B(hexColor >> 8);
		output.A(hexColor);
	}
}

//-----------------------------------------------------------------------
static std::vector<BoneDesc> ReadBones(rapidjson::Value const& bonesDOM)
{
	if (!bonesDOM.IsArray() || bonesDOM.Empty()) {
		///@todo Not implemented
		// Error
		return {};
	}

	std::vector<BoneDesc> bones;
	bones.reserve(bonesDOM.Size());

	for (rapidjson::SizeType index = 0; index < bonesDOM.Size(); ++index)
	{
		auto & boneDOM = bonesDOM[index];
		
		if (!boneDOM.IsObject()) {
			///@todo Not implemented
			// Error
			continue;
		}
		
		if (!boneDOM.HasMember("name") || !boneDOM["name"].IsString()) {
			///@todo Not implemented
			// Error
			continue;
		}
		
		BoneDesc boneDesc;
		
		boneDesc.Name = boneDOM["name"].GetString();
		boneDesc.Pose.Translate.X = 0;
		boneDesc.Pose.Translate.Y = 0;
		boneDesc.Pose.Scale = 1;
		boneDesc.Pose.Rotation = 0;
		
		ReadJsonMember(boneDOM, "parent", boneDesc.Parent);
		//ReadJsonMember(boneDOM, "length", boneDesc.Length);
		ReadJsonMember(boneDOM, "x", boneDesc.Pose.Translate.X);
		ReadJsonMember(boneDOM, "y", boneDesc.Pose.Translate.Y);
		ReadJsonMember(boneDOM, "scaleY", boneDesc.Pose.Scale);
		ReadJsonMember(boneDOM, "scaleX", boneDesc.Pose.Scale);
		
		Radian<float> rotation = 0;
		ReadJsonMember(boneDOM, "rotation", rotation);
		boneDesc.Pose.Rotation = rotation.value;
		
		bones.push_back(std::move(boneDesc));
	}
	return std::move(bones);
}
//-----------------------------------------------------------------------
static std::vector<SlotDesc> ReadSlots(rapidjson::Value const& slotsDOM)
{
	if (!slotsDOM.IsArray()) {
		///@todo Not implemented
		// Error
		return {};
	}
	
	std::vector<SlotDesc> slots;
	slots.reserve(slotsDOM.Size());
	
	for (rapidjson::SizeType index = 0; index < slotsDOM.Size(); ++index)
	{
		auto & slot = slotsDOM[index];
		if (!slot.IsObject()
			|| !slot.HasMember("name")
			|| !slot.HasMember("bone")
			|| !slot.HasMember("attachment")) {
			///@todo Not implemented
			// Error
			continue;
		}
		auto & name = slot["name"];
		auto & bone = slot["bone"];
		auto & attachment = slot["attachment"];
		
		if (!name.IsString()
			|| !bone.IsString()
			|| !attachment.IsString()) {
			///@todo Not implemented
			// Error
			continue;
		}
		
		SlotDesc slotDesc;
		slotDesc.Name = name.GetString();
		slotDesc.Bone = bone.GetString();
		slotDesc.Attachement = attachment.GetString();
		slotDesc.Color = Color::White;

		ReadJsonMember(slot, "color", slotDesc.Color);
	
		slots.push_back(std::move(slotDesc));
	}
	
	return std::move(slots);
}
//-----------------------------------------------------------------------
static std::vector<SkinnedMeshVertexDesc> ReadSkinnedMeshVertices(
	rapidjson::Value const& verticesArray, rapidjson::Value const& uvsArray)
{
	POMDOG_ASSERT(verticesArray.IsArray());
	POMDOG_ASSERT(uvsArray.IsArray());

	auto const verticesCount = uvsArray.Size()/2;

	std::vector<SkinnedMeshVertexDesc> vertices;
	vertices.reserve(verticesCount);
	
	std::uint32_t verticesIter = 0;
	auto uvsIter = uvsArray.Begin();
	
	while (uvsIter != uvsArray.End())
	{
		POMDOG_ASSERT(verticesIter < verticesArray.Size());

		struct LocalVertex {
			Vector2 Position;
			float Weight;
			JointIndex JointIndex;
		};
		
		POMDOG_ASSERT(verticesArray[verticesIter].IsUint());
		std::uint8_t const sourceBoneCount = verticesArray[verticesIter].GetUint();
		++verticesIter;
		POMDOG_ASSERT(sourceBoneCount > 0);
		
		std::vector<LocalVertex> localVertices;
		localVertices.reserve(sourceBoneCount);
	
		for (std::uint8_t index = 0; index < sourceBoneCount; ++index)
		{
			POMDOG_ASSERT(verticesIter < verticesArray.Size());
			
			LocalVertex vertex;
			
			POMDOG_ASSERT(verticesArray[verticesIter].IsUint());
			vertex.JointIndex = verticesArray[verticesIter].GetUint();
			++verticesIter;
			
			POMDOG_ASSERT(verticesArray[verticesIter].IsNumber());
			vertex.Position.X = verticesArray[verticesIter].GetDouble();
			++verticesIter;
			
			POMDOG_ASSERT(verticesArray[verticesIter].IsNumber());
			vertex.Position.Y = verticesArray[verticesIter].GetDouble();
			++verticesIter;
			
			POMDOG_ASSERT(verticesArray[verticesIter].IsNumber());
			vertex.Weight = verticesArray[verticesIter].GetDouble();
			++verticesIter;
		}
	
		SkinnedMeshVertexDesc vertex;
		
		if (localVertices.size() >= vertex.BoundJoints.size())
		{
			std::sort(std::begin(localVertices), std::end(localVertices),
				[](LocalVertex const& a, LocalVertex const& b){ return a.Weight > b.Weight; });

			float accumulatedWeight = 0;
			for (std::uint8_t index = 0; index < vertex.BoundJoints.size(); ++index)
			{
				POMDOG_ASSERT(index < vertex.BoundJoints.size());
				accumulatedWeight += localVertices[index].Weight;
			}
			
			if (accumulatedWeight < 1)
			{
				auto weightDiff = (1 - accumulatedWeight)/vertex.BoundJoints.size();
				for (auto & localVertex: localVertices)
				{
					localVertex.Weight += weightDiff;
				}
			}
		}
		
		vertex.Position = localVertices.front().Position;
		
		auto boneCount = std::min(vertex.BoundJoints.size(), localVertices.size());
		for (std::uint8_t index = 0; index < boneCount; ++index)
		{
			POMDOG_ASSERT(boneCount < vertex.BoundJoints.size());
			vertex.BoundJoints[boneCount] = localVertices[boneCount].JointIndex;
			
			POMDOG_ASSERT(boneCount < vertex.Weight.size());
			vertex.Weight[boneCount] = localVertices[boneCount].Weight;
		}
		
		POMDOG_ASSERT(uvsIter->IsNumber());
		vertex.TextureCoordinate.X = uvsIter->GetDouble();
		++uvsIter;
		POMDOG_ASSERT(uvsIter != uvsArray.End());
		POMDOG_ASSERT(uvsIter->IsNumber());
		vertex.TextureCoordinate.Y = uvsIter->GetDouble();
		++uvsIter;
		
		vertices.push_back(std::move(vertex));
	}
	
	return std::move(vertices);
}
//-----------------------------------------------------------------------
static std::vector<std::uint32_t> ReadSkinnedMeshIndices(rapidjson::Value const& indicesArray)
{
	POMDOG_ASSERT(indicesArray.IsArray());

	std::vector<std::uint32_t> indices;
	indices.reserve(indicesArray.Size());
	
	for (auto iter = indicesArray.Begin(); iter != indicesArray.End(); ++iter)
	{
		POMDOG_ASSERT(iter->IsUint());
		indices.push_back(iter->GetUint());
	}
	
	return std::move(indices);
}
//-----------------------------------------------------------------------
static AttachmentDesc ReadAttachment(rapidjson::Value::ConstMemberIterator const& iter)
{
	POMDOG_ASSERT(iter->name.IsString());
	POMDOG_ASSERT(iter->value.IsObject());

	AttachmentDesc attachmentDesc;
	attachmentDesc.Name = iter->name.GetString();
	attachmentDesc.Scale = {1, 1};
	attachmentDesc.Translate = {1, 1};
	attachmentDesc.Rotation = 0;
	attachmentDesc.Height = 1;
	attachmentDesc.Width = 1;
	
	auto & attachmentObject = iter->value;
	
	ReadJsonMember(attachmentObject, "x", attachmentDesc.Translate.X);
	ReadJsonMember(attachmentObject, "y", attachmentDesc.Translate.Y);
	ReadJsonMember(attachmentObject, "scaleX", attachmentDesc.Scale.X);
	ReadJsonMember(attachmentObject, "scaleY", attachmentDesc.Scale.Y);
	ReadJsonMember(attachmentObject, "rotation", attachmentDesc.Rotation);
	ReadJsonMember(attachmentObject, "width", attachmentDesc.Width);
	ReadJsonMember(attachmentObject, "height", attachmentDesc.Height);
	
	return std::move(attachmentDesc);
}
//-----------------------------------------------------------------------
static SkinnedMeshAttachmentDesc ReadSkinnedMeshAttachment(rapidjson::Value::ConstMemberIterator const& iter)
{
	POMDOG_ASSERT(iter->name.IsString());
	POMDOG_ASSERT(iter->value.IsObject());

	auto & attachmentObject = iter->value;
	
	POMDOG_ASSERT(!(!attachmentObject.HasMember("type")
		|| !attachmentObject["type"].IsString()
		|| std::strcmp(attachmentObject["type"].GetString(), "skinnedmesh") != 0));
	if (!attachmentObject.HasMember("type")
		|| !attachmentObject["type"].IsString()
		|| std::strcmp(attachmentObject["type"].GetString(), "skinnedmesh") != 0) {
		///@todo Not implemented
		// Error
		return {};
	}
	
	POMDOG_ASSERT(!(!attachmentObject.HasMember("triangles") || !attachmentObject["triangles"].IsArray()));
	if (!attachmentObject.HasMember("triangles") || !attachmentObject["triangles"].IsArray()) {
		///@todo Not implemented
		// Error
		return {};
	}
	
	POMDOG_ASSERT(!(!attachmentObject.HasMember("vertices") || !attachmentObject["vertices"].IsArray()));
	if (!attachmentObject.HasMember("vertices") || !attachmentObject["vertices"].IsArray()) {
		///@todo Not implemented
		// Error
		return {};
	}
	
	POMDOG_ASSERT(!(!attachmentObject.HasMember("uvs") || !attachmentObject["uvs"].IsArray()));
	if (!attachmentObject.HasMember("uvs") || !attachmentObject["uvs"].IsArray()) {
		///@todo Not implemented
		// Error
		return {};
	}
	
	SkinnedMeshAttachmentDesc desc;
	desc.Name = iter->name.GetString();
	desc.Vertices = ReadSkinnedMeshVertices(attachmentObject["vertices"], attachmentObject["uvs"]);
	desc.Indices = ReadSkinnedMeshIndices(attachmentObject["triangles"]);

	return std::move(desc);
}
//-----------------------------------------------------------------------
static std::vector<SkinSlotDesc> ReadSkinSlots(rapidjson::Value const& slotsDOM)
{
	POMDOG_ASSERT(slotsDOM.IsObject());

	std::vector<SkinSlotDesc> slots;

	for (auto slotIter = slotsDOM.MemberBegin(); slotIter != slotsDOM.MemberEnd(); ++slotIter)
	{
		if (!slotIter->name.IsString() || !slotIter->value.IsObject()) {
			///@todo Not implemented
			// Error
			continue;
		}
		
		SkinSlotDesc slotDesc;
		slotDesc.Name = slotIter->name.GetString();
		
		auto & attachmentsDOM = slotIter->value;
		
		for (auto iter = attachmentsDOM.MemberBegin(); iter != attachmentsDOM.MemberEnd(); ++iter)
		{
			if (!iter->name.IsString() || !iter->value.IsObject()) {
				///@todo Not implemented
				// Error
				continue;
			}
			
			auto & attachmentObject = iter->value;
			
			if (attachmentObject.HasMember("type")
				&& attachmentObject["type"].IsString()
				&& std::strcmp(attachmentObject["type"].GetString(), "skinnedmesh") == 0)
			{
				slotDesc.SkinnedMeshAttachments.push_back(ReadSkinnedMeshAttachment(iter));
			}
			else
			{
				slotDesc.Attachments.push_back(ReadAttachment(iter));
			}
		}
		
		slots.push_back(std::move(slotDesc));
	}
	
	return std::move(slots);
}
//-----------------------------------------------------------------------
static std::vector<SkinDesc> ReadSkins(rapidjson::Value const& skinsDOM)
{
	if (!skinsDOM.IsObject()) {
		///@todo Not implemented
		// Error
		return {};
	}
	
	std::vector<SkinDesc> skins;
	
	for (auto iter = skinsDOM.MemberBegin(); iter != skinsDOM.MemberEnd(); ++iter)
	{
		if (!iter->name.IsString() || !iter->value.IsObject()) {
			///@todo Not implemented
			// Error
			continue;
		}
		
		SkinDesc skinDesc;
		skinDesc.Name = iter->name.GetString();
		skinDesc.Slots = ReadSkinSlots(iter->value);
		skins.push_back(std::move(skinDesc));
	}
	
	return std::move(skins);
}
//-----------------------------------------------------------------------
static std::vector<AnimationSamplePointTranslate> ReadAnimationTranslateSamples(rapidjson::Value const& sampleDOM)
{
	if (!sampleDOM.IsArray()) {
		///@todo Not implemented
		// Error
		return {};
	}
	
	std::vector<AnimationSamplePointTranslate> samplePoints;
	samplePoints.reserve(sampleDOM.Size());
	
	for (auto iter = sampleDOM.Begin(); iter != sampleDOM.End(); ++iter)
	{
		if (!iter->IsObject() || !iter->HasMember("time")) {
			// Error
			continue;
		}
		
		AnimationSamplePointTranslate samplePoint;
		samplePoint.TimeSeconds = 0.0f;
		samplePoint.TranslateX = 0.0f;
		samplePoint.TranslateY = 0.0f;
		samplePoint.Curve = KeyframeCurve::Liener;
		
		ReadJsonMember(*iter, "time", samplePoint.TimeSeconds);
		ReadJsonMember(*iter, "x", samplePoint.TranslateX);
		ReadJsonMember(*iter, "y", samplePoint.TranslateY);
		ReadJsonMember(*iter, "curve", samplePoint.Curve);
		
		samplePoints.push_back(std::move(samplePoint));
	}
	
	return std::move(samplePoints);
}
//-----------------------------------------------------------------------
static std::vector<AnimationSamplePointRotate> ReadAnimationRotateSamples(rapidjson::Value const& sampleDOM)
{
	if (!sampleDOM.IsArray()) {
		///@todo Not implemented
		// Error
		return {};
	}
	
	std::vector<AnimationSamplePointRotate> samplePoints;
	samplePoints.reserve(sampleDOM.Size());
	
	for (auto iter = sampleDOM.Begin(); iter != sampleDOM.End(); ++iter)
	{
		if (!iter->IsObject() || !iter->HasMember("time")) {
			// Error
			continue;
		}
		
		AnimationSamplePointRotate samplePoint;
		samplePoint.TimeSeconds = 0.0f;
		samplePoint.Curve = KeyframeCurve::Liener;
		
		ReadJsonMember(*iter, "time", samplePoint.TimeSeconds);
		ReadJsonMember(*iter, "curve", samplePoint.Curve);
		
		Degree<double> degreeAngle = 0;
		ReadJsonMember(*iter, "angle", degreeAngle);
		while (degreeAngle.value > 180) {
			degreeAngle.value -= 360;
		}
		while (degreeAngle.value < -180) {
			degreeAngle.value += 360;
		}
		samplePoint.Rotation = MathHelper::ToRadians(degreeAngle).value;
		
		samplePoints.push_back(std::move(samplePoint));
	}
	
	return std::move(samplePoints);
}
//-----------------------------------------------------------------------
static std::vector<AnimationSamplePointScale> ReadAnimationScaleSamples(rapidjson::Value const& sampleDOM)
{
	if (!sampleDOM.IsArray()) {
		///@todo Not implemented
		// Error
		return {};
	}
	
	std::vector<AnimationSamplePointScale> samplePoints;
	samplePoints.reserve(sampleDOM.Size());
	
	for (auto iter = sampleDOM.Begin(); iter != sampleDOM.End(); ++iter)
	{
		if (!iter->IsObject() || !iter->HasMember("time")) {
			// Error
			continue;
		}
		
		AnimationSamplePointScale samplePoint;
		samplePoint.TimeSeconds = 0.0f;
		samplePoint.Curve = KeyframeCurve::Liener;
		samplePoint.Scale = 1.0f;
		
		ReadJsonMember(*iter, "time", samplePoint.TimeSeconds);
		ReadJsonMember(*iter, "curve", samplePoint.Curve);
		ReadJsonMember(*iter, "x", samplePoint.Scale);
		ReadJsonMember(*iter, "y", samplePoint.Scale);
		
		samplePoints.push_back(std::move(samplePoint));
	}
	
	return std::move(samplePoints);
}
//-----------------------------------------------------------------------
static std::vector<AnimationSamplePointAttachment> ReadAnimationAttachmentSamples(rapidjson::Value const& sampleDOM)
{
	if (!sampleDOM.IsArray()) {
		///@todo Not implemented
		// Error
		return {};
	}
	
	std::vector<AnimationSamplePointAttachment> samplePoints;
	samplePoints.reserve(sampleDOM.Size());
	
	for (auto iter = sampleDOM.Begin(); iter != sampleDOM.End(); ++iter)
	{
		if (!iter->IsObject() || !iter->HasMember("time")) {
			// Error: Invalid file format
			continue;
		}
		
		if (!iter->HasMember("name")) {
			// Error: Invalid file format
			continue;
		}
		
		AnimationSamplePointAttachment samplePoint;
		samplePoint.TimeSeconds = 0.0f;
		
		ReadJsonMember(*iter, "time", samplePoint.TimeSeconds);
		ReadJsonMember(*iter, "name", samplePoint.AttachmentName);
		
		samplePoints.push_back(std::move(samplePoint));
	}
	
	return std::move(samplePoints);
}
//-----------------------------------------------------------------------
static std::vector<BoneAnimationTrackDesc> ReadBoneAnimationSamples(rapidjson::Value const& document)
{
	if (!document.IsObject()) {
		///@todo Not implemented
		// Error
		return {};
	}
	
	std::vector<BoneAnimationTrackDesc> animationSamples;
	
	for (auto iter = document.MemberBegin(); iter != document.MemberEnd(); ++iter)
	{
		if (!iter->name.IsString() || !iter->value.IsObject()) {
			///@todo Not implemented
			// Error
			continue;
		}
		
		BoneAnimationTrackDesc sample;
		sample.BoneName = iter->name.GetString();
		if (iter->value.HasMember("translate")) {
			sample.TranslateSamples = ReadAnimationTranslateSamples(iter->value["translate"]);
		}
		if (iter->value.HasMember("rotate")) {
			sample.RotateSamples = ReadAnimationRotateSamples(iter->value["rotate"]);
		}
		if (iter->value.HasMember("scale")) {
			sample.ScaleSamples = ReadAnimationScaleSamples(iter->value["scale"]);
		}
		animationSamples.push_back(std::move(sample));
	}
	
	return std::move(animationSamples);
}
//-----------------------------------------------------------------------
static std::vector<SlotAnimationTrackDesc> ReadSlotAnimationSamples(rapidjson::Value const& document)
{
	if (!document.IsObject()) {
		///@todo Not implemented
		// Error: Invalid file format
		return {};
	}
	
	std::vector<SlotAnimationTrackDesc> animationSamples;
	
	for (auto iter = document.MemberBegin(); iter != document.MemberEnd(); ++iter)
	{
		if (!iter->name.IsString() || !iter->value.IsObject()) {
			///@todo Not implemented
			// Error: Invalid file format
			continue;
		}
		
		SlotAnimationTrackDesc sample;
		sample.SlotName = iter->name.GetString();
		if (iter->value.HasMember("attachment")) {
			sample.AttachmentSamples = ReadAnimationAttachmentSamples(iter->value["attachment"]);
		}
		animationSamples.push_back(std::move(sample));
	}
	
	return std::move(animationSamples);
}
//-----------------------------------------------------------------------
static std::vector<AnimationClipDesc> ReadAnimationClips(rapidjson::Value const& document)
{
	if (!document.IsObject()) {
		///@todo Not implemented
		// Error
		return {};
	}
	
	std::vector<AnimationClipDesc> animations;
	
	for (auto iter = document.MemberBegin(); iter != document.MemberEnd(); ++iter)
	{
		if (!iter->name.IsString() || !iter->value.IsObject()) {
			///@todo Not implemented
			// Error
			continue;
		}
		
		AnimationClipDesc animationClip;
		animationClip.Name = iter->name.GetString();
		if (iter->value.HasMember("bones")) {
			animationClip.BoneTracks = ReadBoneAnimationSamples(iter->value["bones"]);
		}
		if (iter->value.HasMember("slots")) {
			animationClip.SlotTracks = ReadSlotAnimationSamples(iter->value["slots"]);
		}
		animations.push_back(std::move(animationClip));
	}
	
	return std::move(animations);
}

}// unnamed namespace
//-----------------------------------------------------------------------
Skeletal2D::SkeletonDesc AssetLoader<Skeletal2D::SkeletonDesc>::operator()(AssetLoaderContext const& loaderContext,
	std::string const& assetPath)
{
	auto filename = loaderContext.RootDirectory + "/" + assetPath;
	auto json = ReadBinaryFile(filename);
	
	rapidjson::Document doc;
	doc.Parse<0>(json.data());
	
	if (doc.HasParseError()) {
		///@todo Not implemented
		// Error
		POMDOG_ASSERT(false);
	}
	
	if (!doc.IsObject()) {
		///@todo Not implemented
		// Error
		POMDOG_ASSERT(false);
	}
	
	Skeletal2D::SkeletonDesc skeleton;
	
	if (doc.HasMember("bones")) {
		skeleton.Bones = ReadBones(doc["bones"]);
	}
	if (doc.HasMember("slots")) {
		skeleton.Slots = ReadSlots(doc["slots"]);
	}
	if (doc.HasMember("skins")) {
		skeleton.Skins = ReadSkins(doc["skins"]);
	}
	if (doc.HasMember("animations")) {
		skeleton.AnimationClips = ReadAnimationClips(doc["animations"]);
	}
	
	return std::move(skeleton);
}

}// namespace Details
}// namespace Pomdog
