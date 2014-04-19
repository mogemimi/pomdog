//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "AnimationLoader.hpp"
#include <utility>
#include "AnimationClip.hpp"
#include "AnimationTimeline.hpp"
#include "JointIndex.hpp"
#include "SkeletonDesc.hpp"

namespace Pomdog {
namespace Details {
namespace Skeletal2D {
namespace {

static JointIndex FindJoint(std::vector<BoneDesc> const& bones, std::string const& name)
{
	JointIndex index;

	auto iter = std::find_if(std::begin(bones), std::end(bones),
		[&](Skeletal2D::BoneDesc const& boneDesc){ return boneDesc.Name == name; });
	
	if (iter != std::end(bones)) {
		index = std::distance(std::begin(bones), iter);
	}
	return index;
}

}// unnamed namespace
//-----------------------------------------------------------------------
AnimationClip CreateAnimationClip(SkeletonDesc const& desc)
{
	if (desc.AnimationClips.empty()) {
		return {};
	}
	
	auto & animationClip = desc.AnimationClips.front();
	
	std::vector<std::unique_ptr<AnimationTimeline>> timelines;
	
	for (auto sample: animationClip.Samples)
	{
		auto jointIndex = FindJoint(desc.Bones, sample.BoneName);
		POMDOG_ASSERT(jointIndex);
		
		if (!sample.RotateSamples.empty())
		{
			std::vector<RotateControlPoint> points;
			points.reserve(sample.RotateSamples.size());
			
			for (auto & rotateSample: sample.RotateSamples)
			{
				RotateControlPoint point;
				point.Rotation = rotateSample.Rotation;
				point.TimeSeconds = rotateSample.TimeSeconds;
				points.push_back(std::move(point));
			}
			
			std::sort(std::begin(points), std::end(points),
				[](RotateControlPoint const& a, RotateControlPoint const& b) {
					return a.TimeSeconds < b.TimeSeconds;
				});
			std::unique_ptr<RotateTimeline> timeline(new RotateTimeline(std::move(points), std::move(jointIndex)));
			timelines.push_back(std::move(timeline));
		}
		
		if (!sample.TranslateSamples.empty())
		{
			std::vector<TranslateControlPoint> points;
			points.reserve(sample.TranslateSamples.size());
			
			for (auto & translateSample: sample.TranslateSamples)
			{
				TranslateControlPoint point;
				point.TranslateX = translateSample.TranslateX;
				point.TranslateY = translateSample.TranslateY;
				point.TimeSeconds = translateSample.TimeSeconds;
				points.push_back(std::move(point));
			}
			
			std::sort(std::begin(points), std::end(points),
				[](TranslateControlPoint const& a, TranslateControlPoint const& b) {
					return a.TimeSeconds < b.TimeSeconds;
				});
			std::unique_ptr<TranslateTimeline> timeline(new TranslateTimeline(std::move(points), std::move(jointIndex)));
			timelines.push_back(std::move(timeline));
		}
	}
	
	return AnimationClip(std::move(timelines));
}

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog
