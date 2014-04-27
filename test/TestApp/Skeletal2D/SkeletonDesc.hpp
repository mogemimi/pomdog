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
#include <string>
#include <Pomdog/Pomdog.hpp>
#include "CompressedFloat.hpp"

namespace Pomdog {
namespace Details {
namespace Skeletal2D {

class JointPose {
public:
	Vector2 Translate;
	Radian<float> Rotation;
	float Scale;
};

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
	std::string Bone;
	Color Color;
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

class SkinSlotDesc {
public:
	std::vector<AttachmentDesc> Attachments;
	std::string Name;
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

class BoneAnimationSample {
public:
	std::string BoneName;
	std::vector<AnimationSamplePointTranslate> TranslateSamples;
	std::vector<AnimationSamplePointRotate> RotateSamples;
	std::vector<AnimationSamplePointScale> ScaleSamples;
};

class BoneAnimationClip {
public:
	std::string Name;
	std::vector<BoneAnimationSample> Samples;
};

class SkeletonDesc {
public:
	std::vector<BoneDesc> Bones;
	std::vector<SlotDesc> Slots;
	std::vector<SkinDesc> Skins;
	std::vector<BoneAnimationClip> AnimationClips;
};

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SKELETONDESC_BCCD55FC_EEDC_43CB_AC5F_194D7A58686D_HPP)
