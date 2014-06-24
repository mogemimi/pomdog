//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "AnimationTrack.hpp"
#include <algorithm>
#include <Pomdog/Math/MathHelper.hpp>
#include "RotationTrack.hpp"
#include "ScaleTrack.hpp"
#include "TranslationTrack.hpp"
#include "Skeleton.hpp"
#include "SkeletonPose.hpp"
#include "AnimationKeyHelper.hpp"

namespace Pomdog {
namespace Details {
namespace Skeletal2D {
namespace {

template <typename ForwardIterator, typename T>
static
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

}// unnamed namespace
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - RotationTrack
#endif
//-----------------------------------------------------------------------
RotationTrack::RotationTrack(std::vector<RotationKeyframe> && keysIn, JointIndex && jointIndexIn)
	: keys(std::move(keysIn))
	, jointIndex(std::move(jointIndexIn))
{
	POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<RotationKeyframe>));
}
//-----------------------------------------------------------------------
void RotationTrack::Apply(DurationSeconds const& time, Skeleton const& skeleton, SkeletonPose & skeletonPose)
{
	RotationKeyframe point;
	point.Time = time;
	
	POMDOG_ASSERT(!keys.empty());
	auto pointPair = BinarySearchNearestPoints(std::begin(keys), std::end(keys), point);
	
	auto & joint = skeleton.Joints(jointIndex);
	auto & bindPose = joint.BindPose;
	
	POMDOG_ASSERT(*jointIndex < skeletonPose.LocalPose.size());
	auto & localPose = skeletonPose.LocalPose[*jointIndex];
	
	if (pointPair.first == pointPair.second)
	{
		POMDOG_ASSERT((time <= keys.front().Time) || (time >= keys.back().Time));
		
		auto diff = pointPair.first->Rotation.ToFloat();
		POMDOG_ASSERT(diff <= MathConstants<float>::Pi());
		POMDOG_ASSERT(diff >= -MathConstants<float>::Pi());
		localPose.Rotation = bindPose.Rotation + diff;
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
	
	POMDOG_ASSERT(rotation1 <= MathConstants<float>::Pi());
	POMDOG_ASSERT(rotation1 >= -MathConstants<float>::Pi());
	POMDOG_ASSERT(rotation2 <= MathConstants<float>::Pi());
	POMDOG_ASSERT(rotation2 >= -MathConstants<float>::Pi());

	localPose.Rotation = bindPose.Rotation + MathHelper::Lerp(rotation1, rotation2, amount);
}
//-----------------------------------------------------------------------
DurationSeconds RotationTrack::Length() const
{
	POMDOG_ASSERT(!keys.empty());
	POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<RotationKeyframe>));
	POMDOG_ASSERT(keys.front().Time <= keys.back().Time);
	return DurationSeconds(keys.back().Time);
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - ScaleTrack
#endif
//-----------------------------------------------------------------------
ScaleTrack::ScaleTrack(std::vector<ScaleKeyframe> && keysIn, JointIndex && jointIndexIn)
	: keys(std::move(keysIn))
	, jointIndex(std::move(jointIndexIn))
{
	POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<ScaleKeyframe>));
}
//-----------------------------------------------------------------------
void ScaleTrack::Apply(DurationSeconds const& time, Skeleton const& skeleton, SkeletonPose & skeletonPose)
{
	ScaleKeyframe point;
	point.Time = time;
	
	POMDOG_ASSERT(!keys.empty());
	auto pointPair = BinarySearchNearestPoints(std::begin(keys), std::end(keys), point);
	
	auto & joint = skeleton.Joints(jointIndex);
	auto & bindPose = joint.BindPose;
	
	POMDOG_ASSERT(*jointIndex < skeletonPose.LocalPose.size());
	auto & localPose = skeletonPose.LocalPose[*jointIndex];
	
	if (pointPair.first == pointPair.second)
	{
		POMDOG_ASSERT((time <= keys.front().Time) || (time >= keys.back().Time));
		
		localPose.Scale = bindPose.Scale * pointPair.first->Scale.ToFloat();
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
	localPose.Scale = bindPose.Scale * MathHelper::Lerp(scale1, scale2, amount);
}
//-----------------------------------------------------------------------
DurationSeconds ScaleTrack::Length() const
{
	POMDOG_ASSERT(!keys.empty());
	POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<ScaleKeyframe>));
	POMDOG_ASSERT(keys.front().Time <= keys.back().Time);
	return DurationSeconds(keys.back().Time);
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - TranslationTrack
#endif
//-----------------------------------------------------------------------
TranslationTrack::TranslationTrack(std::vector<TranslationKeyframe> && keysIn, JointIndex && jointIndexIn)
	: keys(std::move(keysIn))
	, jointIndex(std::move(jointIndexIn))
{
	POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<TranslationKeyframe>));
}
//-----------------------------------------------------------------------
void TranslationTrack::Apply(DurationSeconds const& time, Skeleton const& skeleton, SkeletonPose & skeletonPose)
{
	TranslationKeyframe point;
	point.Time = time;
	
	POMDOG_ASSERT(!keys.empty());
	auto pointPair = BinarySearchNearestPoints(std::begin(keys), std::end(keys), point);
		
	auto & joint = skeleton.Joints(jointIndex);
	auto & bindPose = joint.BindPose;
	
	POMDOG_ASSERT(*jointIndex < skeletonPose.LocalPose.size());
	auto & localPose = skeletonPose.LocalPose[*jointIndex];
	
	if (pointPair.first == pointPair.second)
	{
		POMDOG_ASSERT((time <= keys.front().Time) || (time >= keys.back().Time));
		
		auto diff = Vector2{pointPair.first->TranslateX.ToFloat(), pointPair.first->TranslateY.ToFloat()};
		localPose.Translate = bindPose.Translate + diff;
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
	
	localPose.Translate = bindPose.Translate + Vector2::Lerp(translate1, translate2, amount);
}
//-----------------------------------------------------------------------
DurationSeconds TranslationTrack::Length() const
{
	POMDOG_ASSERT(!keys.empty());
	POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<TranslationKeyframe>));
	POMDOG_ASSERT(keys.front().Time <= keys.back().Time);
	return DurationSeconds(keys.back().Time);
}

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog
