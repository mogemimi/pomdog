//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "AnimationTimeline.hpp"
#include <algorithm>
#include "Skeleton.hpp"

namespace Pomdog {
namespace Details {
namespace Skeletal2D {
namespace {

template <typename ForwardIterator, typename T>
static
std::pair<ForwardIterator, ForwardIterator> BinarySearchNearestPoints(ForwardIterator first, ForwardIterator last, T const& value)
{
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
RotateTimeline::RotateTimeline(std::vector<RotateControlPoint> && pointsIn, JointIndex && jointIndexIn)
	: points(std::move(pointsIn))
	, jointIndex(std::move(jointIndexIn))
{
	POMDOG_ASSERT(std::is_sorted(std::begin(points), std::end(points),
		[](RotateControlPoint const& a, RotateControlPoint const& b) {
			return a.TimeSeconds < b.TimeSeconds;
		}));
}
//-----------------------------------------------------------------------
void RotateTimeline::Apply(Skeleton & skeleton, DurationSeconds const& time)
{
	POMDOG_ASSERT(!points.empty());
	
	if (points.back().TimeSeconds < time.count()) {
		///@todo Not implemented
		return;
	}

	RotateControlPoint point;
	point.TimeSeconds = time.count();
	
	POMDOG_ASSERT(!points.empty());
	auto pointPair = BinarySearchNearestPoints(std::begin(points), std::end(points), point);
	
	//POMDOG_ASSERT(pointPair.first->TimeSeconds < pointPair.second->TimeSeconds);
	POMDOG_ASSERT(pointPair.first->TimeSeconds <= time.count());
	POMDOG_ASSERT(pointPair.second->TimeSeconds >= time.count());
	
	auto diffTime = (time.count() - pointPair.first->TimeSeconds);
	auto frameTime = pointPair.second->TimeSeconds - pointPair.first->TimeSeconds;
	
	POMDOG_ASSERT(frameTime != 0.0f);
	auto amount =  diffTime / frameTime;
	
	auto & joint = skeleton.Joints(jointIndex);
	auto & bindPose = skeleton.BindPoses()[*jointIndex];
	
	auto a = pointPair.first->Rotation.ToFloat();
	auto b = pointPair.second->Rotation.ToFloat();
	
	POMDOG_ASSERT(a <= MathConstants<float>::Pi());
	POMDOG_ASSERT(a >= -MathConstants<float>::Pi());
	POMDOG_ASSERT(b <= MathConstants<float>::Pi());
	POMDOG_ASSERT(b >= -MathConstants<float>::Pi());
		
	joint.Rotation = ((bindPose.Rotation + a) * (1.0f - amount)) + ((bindPose.Rotation + b) * amount);
}
//-----------------------------------------------------------------------
TranslateTimeline::TranslateTimeline(std::vector<TranslateControlPoint> && pointsIn, JointIndex && jointIndexIn)
	: points(std::move(pointsIn))
	, jointIndex(std::move(jointIndexIn))
{
	POMDOG_ASSERT(std::is_sorted(std::begin(points), std::end(points),
		[](TranslateControlPoint const& a, TranslateControlPoint const& b) {
			return a.TimeSeconds < b.TimeSeconds;
		}));
}
//-----------------------------------------------------------------------
void TranslateTimeline::Apply(Skeleton & skeleton, DurationSeconds const& time)
{
	POMDOG_ASSERT(!points.empty());
	
	if (points.back().TimeSeconds < time.count()) {
		///@todo Not implemented
		return;
	}
	
	TranslateControlPoint point;
	point.TimeSeconds = time.count();
	
	POMDOG_ASSERT(!points.empty());
	auto pointPair = BinarySearchNearestPoints(std::begin(points), std::end(points), point);
	
	//POMDOG_ASSERT(pointPair.first->TimeSeconds < pointPair.second->TimeSeconds);
	POMDOG_ASSERT(pointPair.first->TimeSeconds <= time.count());
	POMDOG_ASSERT(pointPair.second->TimeSeconds >= time.count());
	
	auto diffTime = (time.count() - pointPair.first->TimeSeconds);
	auto frameTime = pointPair.second->TimeSeconds - pointPair.first->TimeSeconds;
	
	POMDOG_ASSERT(frameTime != 0.0f);
	auto amount =  diffTime / frameTime;
	
	auto & joint = skeleton.Joints(jointIndex);
	auto & bindPose = skeleton.BindPoses()[*jointIndex];
	
	auto a = Vector2{pointPair.first->TranslateX.ToFloat(), pointPair.first->TranslateY.ToFloat()};
	auto b = Vector2{pointPair.second->TranslateX.ToFloat(), pointPair.second->TranslateY.ToFloat()};
	
	joint.Translate = ((bindPose.Translate + a) * (1.0f - amount)) + ((bindPose.Translate + b) * amount);
}

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog
