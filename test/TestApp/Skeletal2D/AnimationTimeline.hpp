//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ANIMATIONTIMELINE_5624300F_C197_46B6_B428_BDF5D597DB9F_HPP
#define POMDOG_ANIMATIONTIMELINE_5624300F_C197_46B6_B428_BDF5D597DB9F_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>
#include "CompressedFloat.hpp"
#include "JointIndex.hpp"

namespace Pomdog {

class Skeleton;

class AnimationTimeline {
public:
	virtual ~AnimationTimeline() = default;
	virtual void Apply(Skeleton & skeleton, DurationSeconds const& time) = 0;
};

namespace Details {
namespace Skeletal2D {

class RotateControlPoint {
public:
	CompressedFloat<std::int16_t, 4500> Rotation;
	float TimeSeconds;
	
	bool operator<(RotateControlPoint const& other) const
	{
		return TimeSeconds < other.TimeSeconds;
	}
	
	static_assert(decltype(Rotation)::Max() > 3.1416f*2.0f, "");
	static_assert(decltype(Rotation)::Min() < -3.1416f*2.0f, "");
};

class RotateTimeline final : public AnimationTimeline {
public:
	RotateTimeline(std::vector<RotateControlPoint> && pointsIn, JointIndex && jointIndexIn);

	void Apply(Skeleton & skeleton, DurationSeconds const& time);
	
private:
	std::vector<RotateControlPoint> points;
	JointIndex jointIndex;
};


class TranslateControlPoint {
public:
	CompressedFloat<std::int16_t, 7> TranslateX;
	CompressedFloat<std::int16_t, 7> TranslateY;
	float TimeSeconds;
	
	bool operator<(TranslateControlPoint const& other) const
	{
		return TimeSeconds < other.TimeSeconds;
	}
	
	static_assert(decltype(TranslateX)::Max() > 1024.0f, "");
	static_assert(decltype(TranslateX)::Min() < -1024.0f, "");
	static_assert(decltype(TranslateY)::Max() > 1024.0f, "");
	static_assert(decltype(TranslateY)::Min() < -1024.0f, "");
};


class TranslateTimeline final : public AnimationTimeline {
public:
	TranslateTimeline(std::vector<TranslateControlPoint> && pointsIn, JointIndex && jointIndexIn);

	void Apply(Skeleton & skeleton, DurationSeconds const& time);
	
private:
	std::vector<TranslateControlPoint> points;
	JointIndex jointIndex;
};

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONTIMELINE_5624300F_C197_46B6_B428_BDF5D597DB9F_HPP)
