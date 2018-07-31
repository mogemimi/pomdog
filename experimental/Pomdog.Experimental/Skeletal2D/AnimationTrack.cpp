// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "AnimationTrack.hpp"
#include "detail/AnimationKeyHelper.hpp"
#include "RotationTrack.hpp"
#include "ScaleTrack.hpp"
#include "TranslationTrack.hpp"
#include "Skeleton.hpp"
#include "SkeletonPose.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include <algorithm>

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {
namespace {

template <typename ForwardIterator, typename T>
std::pair<ForwardIterator, ForwardIterator> BinarySearchNearestPoints(ForwardIterator first, ForwardIterator last, T const& value)
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

} // unnamed namespace

// MARK: - RotationTrack

RotationTrack::RotationTrack(std::vector<RotationKeyframe> && keysIn, JointIndex && jointIndexIn)
    : keys(std::move(keysIn))
    , jointIndex(std::move(jointIndexIn))
{
    POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<RotationKeyframe>));
}

void RotationTrack::Apply(AnimationTimeInterval const& time, Skeleton const& skeleton, SkeletonPose & skeletonPose)
{
    RotationKeyframe point;
    point.Time = time;

    POMDOG_ASSERT(!keys.empty());
    auto pointPair = BinarySearchNearestPoints(std::begin(keys), std::end(keys), point);

    auto & joint = skeleton.Joints(jointIndex);
    auto & bindPose = joint.BindPose;

    POMDOG_ASSERT(*jointIndex < skeletonPose.JointPoses.size());
    auto & pose = skeletonPose.JointPoses[*jointIndex];

    if (pointPair.first == pointPair.second)
    {
        POMDOG_ASSERT((time <= keys.front().Time) || (time >= keys.back().Time));

        auto diff = pointPair.first->Rotation.ToFloat();
        POMDOG_ASSERT(diff <= Math::Pi<float>);
        POMDOG_ASSERT(diff >= -Math::Pi<float>);
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
    float amount =  diffTime / frameTime;
    auto rotation1 = pointPair.first->Rotation.ToFloat();
    auto rotation2 = pointPair.second->Rotation.ToFloat();

    POMDOG_ASSERT(rotation1 <= Math::Pi<float>);
    POMDOG_ASSERT(rotation1 >= -Math::Pi<float>);
    POMDOG_ASSERT(rotation2 <= Math::Pi<float>);
    POMDOG_ASSERT(rotation2 >= -Math::Pi<float>);

    pose.Rotation = bindPose.Rotation + MathHelper::Lerp(rotation1, rotation2, amount);
}

AnimationTimeInterval RotationTrack::Length() const
{
    POMDOG_ASSERT(!keys.empty());
    POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<RotationKeyframe>));
    POMDOG_ASSERT(keys.front().Time <= keys.back().Time);
    return keys.back().Time;
}

// MARK: - ScaleTrack

ScaleTrack::ScaleTrack(std::vector<ScaleKeyframe> && keysIn, JointIndex && jointIndexIn)
    : keys(std::move(keysIn))
    , jointIndex(std::move(jointIndexIn))
{
    POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<ScaleKeyframe>));
}

void ScaleTrack::Apply(AnimationTimeInterval const& time, Skeleton const& skeleton, SkeletonPose & skeletonPose)
{
    ScaleKeyframe point;
    point.Time = time;

    POMDOG_ASSERT(!keys.empty());
    auto pointPair = BinarySearchNearestPoints(std::begin(keys), std::end(keys), point);

    auto & joint = skeleton.Joints(jointIndex);
    auto & bindPose = joint.BindPose;

    POMDOG_ASSERT(*jointIndex < skeletonPose.JointPoses.size());
    auto & pose = skeletonPose.JointPoses[*jointIndex];

    if (pointPair.first == pointPair.second)
    {
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
    pose.Scale = bindPose.Scale * MathHelper::Lerp(scale1, scale2, amount);
}

AnimationTimeInterval ScaleTrack::Length() const
{
    POMDOG_ASSERT(!keys.empty());
    POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<ScaleKeyframe>));
    POMDOG_ASSERT(keys.front().Time <= keys.back().Time);
    return keys.back().Time;
}

// MARK: - TranslationTrack

TranslationTrack::TranslationTrack(std::vector<TranslationKeyframe> && keysIn, JointIndex && jointIndexIn)
    : keys(std::move(keysIn))
    , jointIndex(std::move(jointIndexIn))
{
    POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<TranslationKeyframe>));
}

void TranslationTrack::Apply(AnimationTimeInterval const& time, Skeleton const& skeleton, SkeletonPose & skeletonPose)
{
    TranslationKeyframe point;
    point.Time = time;

    POMDOG_ASSERT(!keys.empty());
    auto pointPair = BinarySearchNearestPoints(std::begin(keys), std::end(keys), point);

    auto & joint = skeleton.Joints(jointIndex);
    auto & bindPose = joint.BindPose;

    POMDOG_ASSERT(*jointIndex < skeletonPose.JointPoses.size());
    auto & pose = skeletonPose.JointPoses[*jointIndex];

    if (pointPair.first == pointPair.second)
    {
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
    float amount =  diffTime / frameTime;
    auto translate1 = Vector2{pointPair.first->TranslateX.ToFloat(), pointPair.first->TranslateY.ToFloat()};
    auto translate2 = Vector2{pointPair.second->TranslateX.ToFloat(), pointPair.second->TranslateY.ToFloat()};

    pose.Translate = bindPose.Translate + Vector2::Lerp(translate1, translate2, amount);
}

AnimationTimeInterval TranslationTrack::Length() const
{
    POMDOG_ASSERT(!keys.empty());
    POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<TranslationKeyframe>));
    POMDOG_ASSERT(keys.front().Time <= keys.back().Time);
    return keys.back().Time;
}

} // namespace Skeletal2D
} // namespace Detail
} // namespace Pomdog
