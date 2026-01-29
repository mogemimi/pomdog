// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/skeletal2d/animation_time_interval.h"
#include "pomdog/experimental/skeletal2d/compressed_float.h"
#include "pomdog/experimental/skeletal2d/joint_index.h"
#include "pomdog/experimental/skeletal2d/joint_pose.h"
#include "pomdog/math/color.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cstdint>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::spine {

class BoneDesc final {
public:
    std::string Name;
    std::string Parent;
    skeletal2d::JointPose Pose;
};

class SlotDesc final {
public:
    std::string Name;
    std::string Attachement;
    pomdog::Color Color;
    skeletal2d::JointIndex Joint;
};

class AttachmentDesc final {
public:
    std::string Name;
    Vector2 Translate;
    Vector2 Scale;
    Radian<f32> Rotation;
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
    std::array<skeletal2d::JointIndex, 4> Joints;
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
    skeletal2d::AnimationTimeInterval Time;
    skeletal2d::detail::CompressedFloat<std::int16_t, 7> TranslateX;
    skeletal2d::detail::CompressedFloat<std::int16_t, 7> TranslateY;
    KeyframeCurve Curve;

    static_assert(decltype(TranslateX)::Max() > 1024.0f, "");
    static_assert(decltype(TranslateX)::Min() < -1024.0f, "");
    static_assert(decltype(TranslateY)::Max() > 1024.0f, "");
    static_assert(decltype(TranslateY)::Min() < -1024.0f, "");
};

class AnimationSamplePointRotate final {
public:
    skeletal2d::AnimationTimeInterval Time;
    skeletal2d::detail::CompressedFloat<std::int16_t, 4500> Rotation;
    KeyframeCurve Curve;

    static_assert(decltype(Rotation)::Max() > 3.1416f * 2.0f, "");
    static_assert(decltype(Rotation)::Min() < -3.1416f * 2.0f, "");
};

class AnimationSamplePointScale final {
public:
    skeletal2d::AnimationTimeInterval Time;
    skeletal2d::detail::CompressedFloat<std::int16_t, 1500> Scale;
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
    skeletal2d::AnimationTimeInterval Time;
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

} // namespace pomdog::spine
