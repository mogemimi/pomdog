// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Skeletal2D/AnimationTimeInterval.hpp"
#include "Pomdog/Experimental/Skeletal2D/CompressedFloat.hpp"
#include "Pomdog/Experimental/Skeletal2D/JointIndex.hpp"
#include "Pomdog/Experimental/Skeletal2D/JointPose.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Radian.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace Pomdog::Spine {

class BoneDesc final {
public:
    std::string Name;
    std::string Parent;
    Skeletal2D::JointPose Pose;
};

class SlotDesc final {
public:
    std::string Name;
    std::string Attachement;
    Pomdog::Color Color;
    Skeletal2D::JointIndex Joint;
};

class AttachmentDesc final {
public:
    std::string Name;
    Vector2 Translate;
    Vector2 Scale;
    Radian<float> Rotation;
    std::uint16_t Width;
    std::uint16_t Height;
};

class SkinnedMeshVertexDesc final {
public:
    Vector2 Position;
    Vector2 TextureCoordinate;
    ///@note To get the Weights[3] value, run the following code:
    /// auto weight3 = 1 - (Weights[0] + Weights[1] + Weights[2]);
    std::array<float, 3> Weights;
    std::array<Skeletal2D::JointIndex, 4> Joints;
};

class SkinnedMeshAttachmentDesc final {
public:
    std::string Name;
    std::vector<SkinnedMeshVertexDesc> Vertices;
    std::vector<std::uint16_t> Indices;
};

class SkinSlotDesc final {
public:
    std::vector<AttachmentDesc> Attachments;
    std::vector<SkinnedMeshAttachmentDesc> SkinnedMeshAttachments;
    std::string SlotName;
};

class SkinDesc final {
public:
    std::vector<SkinSlotDesc> Slots;
    std::string Name;
};

enum class KeyframeCurve : std::uint8_t {
    Liener,
    Stepped,
    Bezier,
};

class AnimationSamplePointTranslate final {
public:
    Skeletal2D::AnimationTimeInterval Time;
    Skeletal2D::Detail::CompressedFloat<std::int16_t, 7> TranslateX;
    Skeletal2D::Detail::CompressedFloat<std::int16_t, 7> TranslateY;
    KeyframeCurve Curve;

    static_assert(decltype(TranslateX)::Max() > 1024.0f, "");
    static_assert(decltype(TranslateX)::Min() < -1024.0f, "");
    static_assert(decltype(TranslateY)::Max() > 1024.0f, "");
    static_assert(decltype(TranslateY)::Min() < -1024.0f, "");
};

class AnimationSamplePointRotate final {
public:
    Skeletal2D::AnimationTimeInterval Time;
    Skeletal2D::Detail::CompressedFloat<std::int16_t, 4500> Rotation;
    KeyframeCurve Curve;

    static_assert(decltype(Rotation)::Max() > 3.1416f * 2.0f, "");
    static_assert(decltype(Rotation)::Min() < -3.1416f * 2.0f, "");
};

class AnimationSamplePointScale final {
public:
    Skeletal2D::AnimationTimeInterval Time;
    Skeletal2D::Detail::CompressedFloat<std::int16_t, 1500> Scale;
    KeyframeCurve Curve;

    static_assert(decltype(Scale)::Max() > 20.0f, "");
    static_assert(decltype(Scale)::Min() < -20.0f, "");
};

class BoneAnimationTrackDesc final {
public:
    std::string BoneName;
    std::vector<AnimationSamplePointTranslate> TranslateSamples;
    std::vector<AnimationSamplePointRotate> RotateSamples;
    std::vector<AnimationSamplePointScale> ScaleSamples;
};

class AnimationSamplePointAttachment final {
public:
    std::string AttachmentName;
    Skeletal2D::AnimationTimeInterval Time;
};

class SlotAnimationTrackDesc final {
public:
    std::string SlotName;
    std::vector<AnimationSamplePointAttachment> AttachmentSamples;
};

class AnimationClipDesc final {
public:
    std::string Name;
    std::vector<BoneAnimationTrackDesc> BoneTracks;
    std::vector<SlotAnimationTrackDesc> SlotTracks;
};

class SkeletonDesc final {
public:
    std::vector<BoneDesc> Bones;
    std::vector<SlotDesc> Slots;
    std::vector<SkinDesc> Skins;
    std::vector<AnimationClipDesc> AnimationClips;
};

} // namespace Pomdog::Spine
