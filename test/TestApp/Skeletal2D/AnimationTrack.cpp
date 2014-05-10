//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "AnimationTrack.hpp"
#include "RotationTrack.hpp"
#include "ScaleTrack.hpp"
#include "TranslationTrack.hpp"
#include <algorithm>
#include "Skeleton.hpp"
#include "SkeletonPose.hpp"

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

	auto it = std::lower_bound(first, last, value);

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
bool operator<(RotationKeyframe const& a, RotationKeyframe const& b)
{
	return a.TimeSeconds < b.TimeSeconds;
}
//-----------------------------------------------------------------------
bool operator<(ScaleKeyframe const& a, ScaleKeyframe const& b)
{
	return a.TimeSeconds < b.TimeSeconds;
}
//-----------------------------------------------------------------------
bool operator<(TranslationKeyframe const& a, TranslationKeyframe const& b)
{
	return a.TimeSeconds < b.TimeSeconds;
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - RotationTrack
#endif
//-----------------------------------------------------------------------
RotationTrack::RotationTrack(std::vector<RotationKeyframe> && keysIn, JointIndex && jointIndexIn)
	: keys(std::move(keysIn))
	, jointIndex(std::move(jointIndexIn))
{
	POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys)));
}
//-----------------------------------------------------------------------
void RotationTrack::Apply(DurationSeconds const& time, Skeleton const& skeleton, SkeletonPose & skeletonPose)
{
	RotationKeyframe point;
	point.TimeSeconds = time.count();
	
	POMDOG_ASSERT(!keys.empty());
	auto pointPair = BinarySearchNearestPoints(std::begin(keys), std::end(keys), point);
	
	auto & joint = skeleton.Joints(jointIndex);
	auto & bindPose = joint.BindPose;
	
	POMDOG_ASSERT(*jointIndex < skeletonPose.LocalPose.size());
	auto & localPose = skeletonPose.LocalPose[*jointIndex];
	
	if (pointPair.first == pointPair.second)
	{
		POMDOG_ASSERT((time.count() <= keys.front().TimeSeconds)
			|| (time.count() >= keys.back().TimeSeconds));
		
		auto diff = pointPair.first->Rotation.ToFloat();
		POMDOG_ASSERT(diff <= MathConstants<float>::Pi());
		POMDOG_ASSERT(diff >= -MathConstants<float>::Pi());
		localPose.Rotation = bindPose.Rotation + diff;
		return;
	}
	
	POMDOG_ASSERT(pointPair.first != pointPair.second);
	POMDOG_ASSERT(pointPair.first->TimeSeconds < pointPair.second->TimeSeconds);
	POMDOG_ASSERT(pointPair.first->TimeSeconds <= static_cast<float>(time.count()));
	POMDOG_ASSERT(pointPair.second->TimeSeconds >= static_cast<float>(time.count()));
	
	auto diffTime = (time.count() - pointPair.first->TimeSeconds);
	auto frameTime = pointPair.second->TimeSeconds - pointPair.first->TimeSeconds;
	
	POMDOG_ASSERT(frameTime != 0.0f);
	auto amount =  diffTime / frameTime;

	auto a = pointPair.first->Rotation.ToFloat();
	auto b = pointPair.second->Rotation.ToFloat();
	
	POMDOG_ASSERT(a <= MathConstants<float>::Pi());
	POMDOG_ASSERT(a >= -MathConstants<float>::Pi());
	POMDOG_ASSERT(b <= MathConstants<float>::Pi());
	POMDOG_ASSERT(b >= -MathConstants<float>::Pi());
		
	localPose.Rotation = ((bindPose.Rotation + a) * (1.0f - amount)) + ((bindPose.Rotation + b) * amount);
}
//-----------------------------------------------------------------------
DurationSeconds RotationTrack::Length() const
{
	POMDOG_ASSERT(!keys.empty());
	POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys)));
	POMDOG_ASSERT(keys.front().TimeSeconds <= keys.back().TimeSeconds);
	return DurationSeconds(keys.back().TimeSeconds);
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
	POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys)));
}
//-----------------------------------------------------------------------
void ScaleTrack::Apply(DurationSeconds const& time, Skeleton const& skeleton, SkeletonPose & skeletonPose)
{
	ScaleKeyframe point;
	point.TimeSeconds = time.count();
	
	POMDOG_ASSERT(!keys.empty());
	auto pointPair = BinarySearchNearestPoints(std::begin(keys), std::end(keys), point);
	
	auto & joint = skeleton.Joints(jointIndex);
	auto & bindPose = joint.BindPose;
	
	POMDOG_ASSERT(*jointIndex < skeletonPose.LocalPose.size());
	auto & localPose = skeletonPose.LocalPose[*jointIndex];
	
	if (pointPair.first == pointPair.second)
	{
		POMDOG_ASSERT((time.count() <= keys.front().TimeSeconds)
			|| (time.count() >= keys.back().TimeSeconds));
		
		localPose.Scale = bindPose.Scale * pointPair.first->Scale.ToFloat();
		return;
	}
	
	POMDOG_ASSERT(pointPair.first != pointPair.second);
	POMDOG_ASSERT(pointPair.first->TimeSeconds < pointPair.second->TimeSeconds);
	POMDOG_ASSERT(pointPair.first->TimeSeconds <= static_cast<float>(time.count()));
	POMDOG_ASSERT(pointPair.second->TimeSeconds >= static_cast<float>(time.count()));
	
	auto diffTime = (time.count() - pointPair.first->TimeSeconds);
	auto frameTime = pointPair.second->TimeSeconds - pointPair.first->TimeSeconds;
	
	POMDOG_ASSERT(frameTime != 0.0f);
	auto amount =  diffTime / frameTime;
		
	auto a = pointPair.first->Scale.ToFloat();
	auto b = pointPair.second->Scale.ToFloat();
	localPose.Scale = ((bindPose.Scale * a) * (1.0f - amount)) + ((bindPose.Scale * b) * amount);
}
//-----------------------------------------------------------------------
DurationSeconds ScaleTrack::Length() const
{
	POMDOG_ASSERT(!keys.empty());
	POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys)));
	POMDOG_ASSERT(keys.front().TimeSeconds <= keys.back().TimeSeconds);
	return DurationSeconds(keys.back().TimeSeconds);
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
	POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys)));
}
//-----------------------------------------------------------------------
void TranslationTrack::Apply(DurationSeconds const& time, Skeleton const& skeleton, SkeletonPose & skeletonPose)
{
	TranslationKeyframe point;
	point.TimeSeconds = time.count();
	
	POMDOG_ASSERT(!keys.empty());
	auto pointPair = BinarySearchNearestPoints(std::begin(keys), std::end(keys), point);
		
	auto & joint = skeleton.Joints(jointIndex);
	auto & bindPose = joint.BindPose;
	
	POMDOG_ASSERT(*jointIndex < skeletonPose.LocalPose.size());
	auto & localPose = skeletonPose.LocalPose[*jointIndex];
	
	if (pointPair.first == pointPair.second)
	{
		POMDOG_ASSERT((time.count() <= keys.front().TimeSeconds)
			|| (time.count() >= keys.back().TimeSeconds));
		
		auto diff = Vector2{pointPair.first->TranslateX.ToFloat(), pointPair.first->TranslateY.ToFloat()};
		localPose.Translate = bindPose.Translate + diff;
		return;
	}
	
	POMDOG_ASSERT(pointPair.first != pointPair.second);
	POMDOG_ASSERT(pointPair.first->TimeSeconds < pointPair.second->TimeSeconds);
	POMDOG_ASSERT(pointPair.first->TimeSeconds <= static_cast<float>(time.count()));
	POMDOG_ASSERT(pointPair.second->TimeSeconds >= static_cast<float>(time.count()));
	
	auto diffTime = (time.count() - pointPair.first->TimeSeconds);
	auto frameTime = pointPair.second->TimeSeconds - pointPair.first->TimeSeconds;
	
	POMDOG_ASSERT(frameTime != 0.0f);
	auto amount =  diffTime / frameTime;
	
	auto a = Vector2{pointPair.first->TranslateX.ToFloat(), pointPair.first->TranslateY.ToFloat()};
	auto b = Vector2{pointPair.second->TranslateX.ToFloat(), pointPair.second->TranslateY.ToFloat()};
	
	localPose.Translate = ((bindPose.Translate + a) * (1.0f - amount)) + ((bindPose.Translate + b) * amount);
}
//-----------------------------------------------------------------------
DurationSeconds TranslationTrack::Length() const
{
	POMDOG_ASSERT(!keys.empty());
	POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys)));
	POMDOG_ASSERT(keys.front().TimeSeconds <= keys.back().TimeSeconds);
	return DurationSeconds(keys.back().TimeSeconds);
}

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog
