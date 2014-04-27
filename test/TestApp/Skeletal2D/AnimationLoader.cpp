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
#include "AnimationTrack.hpp"
#include "RotationTrack.hpp"
#include "ScaleTrack.hpp"
#include "TranslationTrack.hpp"
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
	
	std::vector<std::unique_ptr<AnimationTrack>> tracks;
	
	for (auto sample: animationClip.Samples)
	{
		auto jointIndex = FindJoint(desc.Bones, sample.BoneName);
		POMDOG_ASSERT(jointIndex);
		
		if (!sample.RotateSamples.empty())
		{
			std::vector<RotationKeyframe> points;
			points.reserve(sample.RotateSamples.size());
			
			for (auto & rotateSample: sample.RotateSamples)
			{
				RotationKeyframe point;
				point.Rotation = rotateSample.Rotation;
				point.TimeSeconds = rotateSample.TimeSeconds;
				points.push_back(std::move(point));
			}
			
			std::sort(std::begin(points), std::end(points));
			std::unique_ptr<RotationTrack> timeline(new RotationTrack(std::move(points), std::move(jointIndex)));
			tracks.push_back(std::move(timeline));
		}
		
		if (!sample.ScaleSamples.empty())
		{
			std::vector<ScaleKeyframe> points;
			points.reserve(sample.ScaleSamples.size());
			
			for (auto & scaleSample: sample.ScaleSamples)
			{
				ScaleKeyframe point;
				point.Scale = scaleSample.Scale;
				point.TimeSeconds = scaleSample.TimeSeconds;
				points.push_back(std::move(point));
			}
			
			std::sort(std::begin(points), std::end(points));
			std::unique_ptr<ScaleTrack> timeline(new ScaleTrack(std::move(points), std::move(jointIndex)));
			tracks.push_back(std::move(timeline));
		}
		
		if (!sample.TranslateSamples.empty())
		{
			std::vector<TranslationKeyframe> points;
			points.reserve(sample.TranslateSamples.size());
			
			for (auto & translateSample: sample.TranslateSamples)
			{
				TranslationKeyframe point;
				point.TranslateX = translateSample.TranslateX;
				point.TranslateY = translateSample.TranslateY;
				point.TimeSeconds = translateSample.TimeSeconds;
				points.push_back(std::move(point));
			}

			std::sort(std::begin(points), std::end(points));
			std::unique_ptr<TranslationTrack> timeline(new TranslationTrack(std::move(points), std::move(jointIndex)));
			tracks.push_back(std::move(timeline));
		}
	}
	
	return AnimationClip(std::move(tracks));
}

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog
