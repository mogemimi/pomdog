//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SKELETONDESC_BCCD55FC_EEDC_43CB_AC5F_194D7A58686D_HPP
#define POMDOG_SKELETONDESC_BCCD55FC_EEDC_43CB_AC5F_194D7A58686D_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>
#include <vector>
#include <array>
#include <string>
#include <Pomdog/Math/Vector2.hpp>
#include <Pomdog/Math/Radian.hpp>
#include <Pomdog/Math/Color.hpp>
#include "CompressedFloat.hpp"
#include "JointPose.hpp"
#include "JointIndex.hpp"

namespace Pomdog {
namespace Details {
namespace Skeletal2D {

class BoneDesc {
public:
	std::string Name;
	std::string Parent;
	JointPose Pose;
};

class SlotDesc {
public:
	std::string Name;
	std::string Attachement;
	Color Color;
	JointIndex Joint;
};

class AttachmentDesc {
public:
	std::string Name;
	Vector2 Translate;
	Vector2 Scale;
	Radian<float> Rotation;
	std::uint16_t Width;
	std::uint16_t Height;
};

class SkinnedMeshVertexDesc {
public:
	Vector2 Position;
	Vector2 TextureCoordinate;
	///@note 最後のウェイト係数は省略している
	/// Weight3 = 1 - (Weights[0] + Weights[1] + Weights[2]);
	std::array<float, 3> Weights;
	std::array<JointIndex, 4> Joints;
};

class SkinnedMeshAttachmentDesc {
public:
	std::string Name;
	std::vector<SkinnedMeshVertexDesc> Vertices;
	std::vector<std::uint32_t> Indices;
};

class SkinSlotDesc {
public:
	std::vector<AttachmentDesc> Attachments;
	std::vector<SkinnedMeshAttachmentDesc> SkinnedMeshAttachments;
	std::string SlotName;
};

class SkinDesc {
public:
	std::vector<SkinSlotDesc> Slots;
	std::string Name;
};

enum class KeyframeCurve: std::uint8_t {
	Liener,
	Stepped,
	Bezier,
};

class AnimationSamplePointTranslate {
public:
	float TimeSeconds;
	CompressedFloat<std::int16_t, 7> TranslateX;
	CompressedFloat<std::int16_t, 7> TranslateY;
	KeyframeCurve Curve;
	
	static_assert(decltype(TranslateX)::Max() > 1024.0f, "");
	static_assert(decltype(TranslateX)::Min() < -1024.0f, "");
	static_assert(decltype(TranslateY)::Max() > 1024.0f, "");
	static_assert(decltype(TranslateY)::Min() < -1024.0f, "");
};

class AnimationSamplePointRotate {
public:
	float TimeSeconds;
	CompressedFloat<std::int16_t, 4500> Rotation;
	KeyframeCurve Curve;
	
	static_assert(decltype(Rotation)::Max() > 3.1416f*2.0f, "");
	static_assert(decltype(Rotation)::Min() < -3.1416f*2.0f, "");
};

class AnimationSamplePointScale {
public:
	float TimeSeconds;
	CompressedFloat<std::int16_t, 1500> Scale;
	KeyframeCurve Curve;
	
	static_assert(decltype(Scale)::Max() > 20.0f, "");
	static_assert(decltype(Scale)::Min() < -20.0f, "");
};

class BoneAnimationTrackDesc {
public:
	std::string BoneName;
	std::vector<AnimationSamplePointTranslate> TranslateSamples;
	std::vector<AnimationSamplePointRotate> RotateSamples;
	std::vector<AnimationSamplePointScale> ScaleSamples;
};

class AnimationSamplePointAttachment {
public:
	std::string AttachmentName;
	float TimeSeconds;
};

class SlotAnimationTrackDesc {
public:
	std::string SlotName;
	std::vector<AnimationSamplePointAttachment> AttachmentSamples;
};

class AnimationClipDesc {
public:
	std::string Name;
	std::vector<BoneAnimationTrackDesc> BoneTracks;
	std::vector<SlotAnimationTrackDesc> SlotTracks;
};

class SkeletonDesc {
public:
	std::vector<BoneDesc> Bones;
	std::vector<SlotDesc> Slots;
	std::vector<SkinDesc> Skins;
	std::vector<AnimationClipDesc> AnimationClips;
};

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SKELETONDESC_BCCD55FC_EEDC_43CB_AC5F_194D7A58686D_HPP)
