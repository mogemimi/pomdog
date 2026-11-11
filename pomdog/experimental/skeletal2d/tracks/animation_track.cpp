// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/tracks/animation_track.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/skeleton.h"
#include "pomdog/experimental/skeletal2d/skeleton_pose.h"
#include "pomdog/experimental/skeletal2d/tracks/animation_key_helper.h"
#include "pomdog/experimental/skeletal2d/tracks/rotation_track.h"
#include "pomdog/experimental/skeletal2d/tracks/scale_track.h"
#include "pomdog/experimental/skeletal2d/tracks/translation_track.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/math/math_functions.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d::detail {
namespace {

template <typename ForwardIterator, typename T>
std::pair<ForwardIterator, ForwardIterator> binarySearchNearestPoints(ForwardIterator first, ForwardIterator last, const T& value)
{
    static_assert(std::is_same<typename std::remove_reference<decltype(*first)>::type, T>::value, "");
    POMDOG_ASSERT(first != last);

    auto it = std::lower_bound(first, last, value, AnimationKeyHelper::less<T>);

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
    : keys_(std::move(keysIn))
    , jointIndex_(std::move(jointIndexIn))
{
    POMDOG_ASSERT(std::is_sorted(std::begin(keys_), std::end(keys_), AnimationKeyHelper::less<RotationKeyframe>));
}

void RotationTrack::apply(
    const AnimationTimeInterval& time,
    const Skeleton& skeleton,
    SkeletonPose& skeletonPose,
    [[maybe_unused]] Skin* skin)
{
    RotationKeyframe point;
    point.time = time;

    POMDOG_ASSERT(!keys_.empty());
    auto pointPair = binarySearchNearestPoints(std::begin(keys_), std::end(keys_), point);

    auto& joint = skeleton.joints(jointIndex_);
    auto& bindPose = joint.bindPose;

    POMDOG_ASSERT(*jointIndex_ < skeletonPose.jointPoses.size());
    auto& pose = skeletonPose.jointPoses[*jointIndex_];

    if (pointPair.first == pointPair.second) {
        POMDOG_ASSERT((time <= keys_.front().time) || (time >= keys_.back().time));

        auto diff = pointPair.first->rotation.toFloat();
        POMDOG_ASSERT(diff <= math::Pi<f32>);
        POMDOG_ASSERT(diff >= -math::Pi<f32>);
        pose.rotation = bindPose.rotation + diff;
        return;
    }

    POMDOG_ASSERT(pointPair.first != pointPair.second);
    POMDOG_ASSERT(pointPair.first->time < pointPair.second->time);
    POMDOG_ASSERT(pointPair.first->time <= time);
    POMDOG_ASSERT(pointPair.second->time >= time);

    auto diffTime = (time - pointPair.first->time);
    auto frameTime = pointPair.second->time - pointPair.first->time;

    POMDOG_ASSERT(frameTime.count() != 0);
    f32 amount = diffTime / frameTime;
    auto rotation1 = pointPair.first->rotation.toFloat();
    auto rotation2 = pointPair.second->rotation.toFloat();

    POMDOG_ASSERT(rotation1 <= math::Pi<f32>);
    POMDOG_ASSERT(rotation1 >= -math::Pi<f32>);
    POMDOG_ASSERT(rotation2 <= math::Pi<f32>);
    POMDOG_ASSERT(rotation2 >= -math::Pi<f32>);

    pose.rotation = bindPose.rotation + math::lerp(rotation1, rotation2, amount);
}

AnimationTimeInterval RotationTrack::getLength() const
{
    POMDOG_ASSERT(!keys_.empty());
    POMDOG_ASSERT(std::is_sorted(std::begin(keys_), std::end(keys_), AnimationKeyHelper::less<RotationKeyframe>));
    POMDOG_ASSERT(keys_.front().time <= keys_.back().time);
    return keys_.back().time;
}

// MARK: - ScaleTrack

ScaleTrack::ScaleTrack(std::vector<ScaleKeyframe>&& keysIn, JointIndex&& jointIndexIn)
    : keys_(std::move(keysIn))
    , jointIndex_(std::move(jointIndexIn))
{
    POMDOG_ASSERT(std::is_sorted(std::begin(keys_), std::end(keys_), AnimationKeyHelper::less<ScaleKeyframe>));
}

void ScaleTrack::apply(
    const AnimationTimeInterval& time,
    const Skeleton& skeleton,
    SkeletonPose& skeletonPose,
    [[maybe_unused]] Skin* skin)
{
    ScaleKeyframe point;
    point.time = time;

    POMDOG_ASSERT(!keys_.empty());
    auto pointPair = binarySearchNearestPoints(std::begin(keys_), std::end(keys_), point);

    auto& joint = skeleton.joints(jointIndex_);
    auto& bindPose = joint.bindPose;

    POMDOG_ASSERT(*jointIndex_ < skeletonPose.jointPoses.size());
    auto& pose = skeletonPose.jointPoses[*jointIndex_];

    if (pointPair.first == pointPair.second) {
        POMDOG_ASSERT((time <= keys_.front().time) || (time >= keys_.back().time));

        pose.scale = bindPose.scale * pointPair.first->scale.toFloat();
        return;
    }

    POMDOG_ASSERT(pointPair.first != pointPair.second);
    POMDOG_ASSERT(pointPair.first->time < pointPair.second->time);
    POMDOG_ASSERT(pointPair.first->time <= time);
    POMDOG_ASSERT(pointPair.second->time >= time);

    auto diffTime = (time - pointPair.first->time);
    auto frameTime = pointPair.second->time - pointPair.first->time;

    POMDOG_ASSERT(frameTime.count() != 0);
    f32 amount = diffTime / frameTime;
    f32 scale1 = pointPair.first->scale.toFloat();
    f32 scale2 = pointPair.second->scale.toFloat();
    pose.scale = bindPose.scale * math::lerp(scale1, scale2, amount);
}

AnimationTimeInterval ScaleTrack::getLength() const
{
    POMDOG_ASSERT(!keys_.empty());
    POMDOG_ASSERT(std::is_sorted(std::begin(keys_), std::end(keys_), AnimationKeyHelper::less<ScaleKeyframe>));
    POMDOG_ASSERT(keys_.front().time <= keys_.back().time);
    return keys_.back().time;
}

// MARK: - TranslationTrack

TranslationTrack::TranslationTrack(std::vector<TranslationKeyframe>&& keysIn, JointIndex&& jointIndexIn)
    : keys_(std::move(keysIn))
    , jointIndex_(std::move(jointIndexIn))
{
    POMDOG_ASSERT(std::is_sorted(std::begin(keys_), std::end(keys_), AnimationKeyHelper::less<TranslationKeyframe>));
}

void TranslationTrack::apply(
    const AnimationTimeInterval& time,
    const Skeleton& skeleton,
    SkeletonPose& skeletonPose,
    [[maybe_unused]] Skin* skin)
{
    TranslationKeyframe point;
    point.time = time;

    POMDOG_ASSERT(!keys_.empty());
    auto pointPair = binarySearchNearestPoints(std::begin(keys_), std::end(keys_), point);

    auto& joint = skeleton.joints(jointIndex_);
    auto& bindPose = joint.bindPose;

    POMDOG_ASSERT(*jointIndex_ < skeletonPose.jointPoses.size());
    auto& pose = skeletonPose.jointPoses[*jointIndex_];

    if (pointPair.first == pointPair.second) {
        POMDOG_ASSERT((time <= keys_.front().time) || (time >= keys_.back().time));

        auto diff = Vector2{pointPair.first->translateX.toFloat(), pointPair.first->translateY.toFloat()};
        pose.translate = bindPose.translate + diff;
        return;
    }

    POMDOG_ASSERT(pointPair.first != pointPair.second);
    POMDOG_ASSERT(pointPair.first->time < pointPair.second->time);
    POMDOG_ASSERT(pointPair.first->time <= time);
    POMDOG_ASSERT(pointPair.second->time >= time);

    auto diffTime = (time - pointPair.first->time);
    auto frameTime = pointPair.second->time - pointPair.first->time;

    POMDOG_ASSERT(frameTime.count() != 0);
    f32 amount = diffTime / frameTime;
    auto translate1 = Vector2{pointPair.first->translateX.toFloat(), pointPair.first->translateY.toFloat()};
    auto translate2 = Vector2{pointPair.second->translateX.toFloat(), pointPair.second->translateY.toFloat()};

    pose.translate = bindPose.translate + math::lerp(translate1, translate2, amount);
}

AnimationTimeInterval TranslationTrack::getLength() const
{
    POMDOG_ASSERT(!keys_.empty());
    POMDOG_ASSERT(std::is_sorted(std::begin(keys_), std::end(keys_), AnimationKeyHelper::less<TranslationKeyframe>));
    POMDOG_ASSERT(keys_.front().time <= keys_.back().time);
    return keys_.back().time;
}

} // namespace pomdog::skeletal2d::detail

namespace pomdog::skeletal2d {

AnimationTrack::~AnimationTrack() = default;

} // namespace pomdog::skeletal2d
