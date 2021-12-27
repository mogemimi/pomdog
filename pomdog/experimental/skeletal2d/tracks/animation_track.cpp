// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/tracks/animation_track.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/skeletal2d/skeleton.h"
#include "pomdog/experimental/skeletal2d/skeleton_pose.h"
#include "pomdog/experimental/skeletal2d/tracks/animation_key_helper.h"
#include "pomdog/experimental/skeletal2d/tracks/rotation_track.h"
#include "pomdog/experimental/skeletal2d/tracks/scale_track.h"
#include "pomdog/experimental/skeletal2d/tracks/translation_track.h"
#include "pomdog/math/math.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d::detail {
namespace {

template <typename ForwardIterator, typename T>
std::pair<ForwardIterator, ForwardIterator> BinarySearchNearestPoints(ForwardIterator first, ForwardIterator last, const T& value)
{
    static_assert(std::is_same<typename std::remove_reference<decltype(*first)>::type, T>::value, "");
    POMDOG_ASSERT(first != last);

    auto it = std::lower_bound(first, last, value, AnimationKeyHelper::Less<T>);

    if (it == last) {
        return std::make_pair(std::prev(last), std::prev(last));
    }
    else if (it == first) {
        return std::make_pair(first, first);
    }
    return std::make_pair(std::prev(it), it);
}

} // namespace

// MARK: - RotationTrack

RotationTrack::RotationTrack(std::vector<RotationKeyframe>&& keysIn, JointIndex&& jointIndexIn)
    : keys(std::move(keysIn))
    , jointIndex(std::move(jointIndexIn))
{
    POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<RotationKeyframe>));
}

void RotationTrack::Apply(
    const AnimationTimeInterval& time,
    const Skeleton& skeleton,
    SkeletonPose& skeletonPose,
    [[maybe_unused]] Skin* skin)
{
    RotationKeyframe point;
    point.Time = time;

    POMDOG_ASSERT(!keys.empty());
    auto pointPair = BinarySearchNearestPoints(std::begin(keys), std::end(keys), point);

    auto& joint = skeleton.Joints(jointIndex);
    auto& bindPose = joint.BindPose;

    POMDOG_ASSERT(*jointIndex < skeletonPose.JointPoses.size());
    auto& pose = skeletonPose.JointPoses[*jointIndex];

    if (pointPair.first == pointPair.second) {
        POMDOG_ASSERT((time <= keys.front().Time) || (time >= keys.back().Time));

        auto diff = pointPair.first->Rotation.ToFloat();
        POMDOG_ASSERT(diff <= math::Pi<float>);
        POMDOG_ASSERT(diff >= -math::Pi<float>);
        pose.Rotation = bindPose.Rotation + diff;
        return;
    }

    POMDOG_ASSERT(pointPair.first != pointPair.second);
    POMDOG_ASSERT(pointPair.first->Time < pointPair.second->Time);
    POMDOG_ASSERT(pointPair.first->Time <= time);
    POMDOG_ASSERT(pointPair.second->Time >= time);

    auto diffTime = (time - pointPair.first->Time);
    auto frameTime = pointPair.second->Time - pointPair.first->Time;

    POMDOG_ASSERT(frameTime.count() != 0);
    float amount = diffTime / frameTime;
    auto rotation1 = pointPair.first->Rotation.ToFloat();
    auto rotation2 = pointPair.second->Rotation.ToFloat();

    POMDOG_ASSERT(rotation1 <= math::Pi<float>);
    POMDOG_ASSERT(rotation1 >= -math::Pi<float>);
    POMDOG_ASSERT(rotation2 <= math::Pi<float>);
    POMDOG_ASSERT(rotation2 >= -math::Pi<float>);

    pose.Rotation = bindPose.Rotation + math::Lerp(rotation1, rotation2, amount);
}

AnimationTimeInterval RotationTrack::GetLength() const
{
    POMDOG_ASSERT(!keys.empty());
    POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<RotationKeyframe>));
    POMDOG_ASSERT(keys.front().Time <= keys.back().Time);
    return keys.back().Time;
}

// MARK: - ScaleTrack

ScaleTrack::ScaleTrack(std::vector<ScaleKeyframe>&& keysIn, JointIndex&& jointIndexIn)
    : keys(std::move(keysIn))
    , jointIndex(std::move(jointIndexIn))
{
    POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<ScaleKeyframe>));
}

void ScaleTrack::Apply(
    const AnimationTimeInterval& time,
    const Skeleton& skeleton,
    SkeletonPose& skeletonPose,
    [[maybe_unused]] Skin* skin)
{
    ScaleKeyframe point;
    point.Time = time;

    POMDOG_ASSERT(!keys.empty());
    auto pointPair = BinarySearchNearestPoints(std::begin(keys), std::end(keys), point);

    auto& joint = skeleton.Joints(jointIndex);
    auto& bindPose = joint.BindPose;

    POMDOG_ASSERT(*jointIndex < skeletonPose.JointPoses.size());
    auto& pose = skeletonPose.JointPoses[*jointIndex];

    if (pointPair.first == pointPair.second) {
        POMDOG_ASSERT((time <= keys.front().Time) || (time >= keys.back().Time));

        pose.Scale = bindPose.Scale * pointPair.first->Scale.ToFloat();
        return;
    }

    POMDOG_ASSERT(pointPair.first != pointPair.second);
    POMDOG_ASSERT(pointPair.first->Time < pointPair.second->Time);
    POMDOG_ASSERT(pointPair.first->Time <= time);
    POMDOG_ASSERT(pointPair.second->Time >= time);

    auto diffTime = (time - pointPair.first->Time);
    auto frameTime = pointPair.second->Time - pointPair.first->Time;

    POMDOG_ASSERT(frameTime.count() != 0);
    float amount = diffTime / frameTime;
    float scale1 = pointPair.first->Scale.ToFloat();
    float scale2 = pointPair.second->Scale.ToFloat();
    pose.Scale = bindPose.Scale * math::Lerp(scale1, scale2, amount);
}

AnimationTimeInterval ScaleTrack::GetLength() const
{
    POMDOG_ASSERT(!keys.empty());
    POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<ScaleKeyframe>));
    POMDOG_ASSERT(keys.front().Time <= keys.back().Time);
    return keys.back().Time;
}

// MARK: - TranslationTrack

TranslationTrack::TranslationTrack(std::vector<TranslationKeyframe>&& keysIn, JointIndex&& jointIndexIn)
    : keys(std::move(keysIn))
    , jointIndex(std::move(jointIndexIn))
{
    POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<TranslationKeyframe>));
}

void TranslationTrack::Apply(
    const AnimationTimeInterval& time,
    const Skeleton& skeleton,
    SkeletonPose& skeletonPose,
    [[maybe_unused]] Skin* skin)
{
    TranslationKeyframe point;
    point.Time = time;

    POMDOG_ASSERT(!keys.empty());
    auto pointPair = BinarySearchNearestPoints(std::begin(keys), std::end(keys), point);

    auto& joint = skeleton.Joints(jointIndex);
    auto& bindPose = joint.BindPose;

    POMDOG_ASSERT(*jointIndex < skeletonPose.JointPoses.size());
    auto& pose = skeletonPose.JointPoses[*jointIndex];

    if (pointPair.first == pointPair.second) {
        POMDOG_ASSERT((time <= keys.front().Time) || (time >= keys.back().Time));

        auto diff = Vector2{pointPair.first->TranslateX.ToFloat(), pointPair.first->TranslateY.ToFloat()};
        pose.Translate = bindPose.Translate + diff;
        return;
    }

    POMDOG_ASSERT(pointPair.first != pointPair.second);
    POMDOG_ASSERT(pointPair.first->Time < pointPair.second->Time);
    POMDOG_ASSERT(pointPair.first->Time <= time);
    POMDOG_ASSERT(pointPair.second->Time >= time);

    auto diffTime = (time - pointPair.first->Time);
    auto frameTime = pointPair.second->Time - pointPair.first->Time;

    POMDOG_ASSERT(frameTime.count() != 0);
    float amount = diffTime / frameTime;
    auto translate1 = Vector2{pointPair.first->TranslateX.ToFloat(), pointPair.first->TranslateY.ToFloat()};
    auto translate2 = Vector2{pointPair.second->TranslateX.ToFloat(), pointPair.second->TranslateY.ToFloat()};

    pose.Translate = bindPose.Translate + math::Lerp(translate1, translate2, amount);
}

AnimationTimeInterval TranslationTrack::GetLength() const
{
    POMDOG_ASSERT(!keys.empty());
    POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<TranslationKeyframe>));
    POMDOG_ASSERT(keys.front().Time <= keys.back().Time);
    return keys.back().Time;
}

} // namespace pomdog::skeletal2d::detail

namespace pomdog::skeletal2d {

AnimationTrack::~AnimationTrack() = default;

} // namespace pomdog::skeletal2d
