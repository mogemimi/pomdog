//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "AnimationLoader.hpp"
#include <utility>
#include <algorithm>
#include "AnimationClip.hpp"
#include "AnimationKeyHelper.hpp"
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
	auto iter = std::find_if(std::begin(bones), std::end(bones),
		[&name](BoneDesc const& desc){ return desc.Name == name; });
	
	if (iter != std::end(bones)) {
		return JointIndex(std::distance(std::begin(bones), iter));
	}
	return {};
}

}// unnamed namespace
//-----------------------------------------------------------------------
AnimationClip CreateAnimationClip(SkeletonDesc const& desc, char const* name)
{
	auto iter = std::find_if(std::begin(desc.AnimationClips), std::end(desc.AnimationClips),
		[name](AnimationClipDesc const& clip){ return clip.Name == name; });

	if (std::end(desc.AnimationClips) == iter) {
		///@todo Not implemented
		// Error: Cannot find animation clip
		return {};
	}
	
	auto & animationClip = *iter;
	
	std::vector<std::unique_ptr<AnimationTrack>> tracks;
	
	for (auto & track: animationClip.BoneTracks)
	{
		auto jointIndex = FindJoint(desc.Bones, track.BoneName);
		POMDOG_ASSERT(jointIndex);
		
		if (!track.RotateSamples.empty())
		{
			std::vector<RotationKeyframe> keys;
			keys.reserve(track.RotateSamples.size());
			
			for (auto & sample: track.RotateSamples)
			{
				RotationKeyframe key;
				key.Rotation = sample.Rotation;
				key.Time = sample.Time;
				keys.push_back(std::move(key));
			}
			
			std::sort(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<RotationKeyframe>);
			std::unique_ptr<RotationTrack> timeline(new RotationTrack(std::move(keys), std::move(jointIndex)));
			tracks.push_back(std::move(timeline));
		}
		
		if (!track.ScaleSamples.empty())
		{
			std::vector<ScaleKeyframe> keys;
			keys.reserve(track.ScaleSamples.size());
			
			for (auto & sample: track.ScaleSamples)
			{
				ScaleKeyframe key;
				key.Scale = sample.Scale;
				key.Time = sample.Time;
				keys.push_back(std::move(key));
			}
			
			std::sort(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<ScaleKeyframe>);
			std::unique_ptr<ScaleTrack> timeline(new ScaleTrack(std::move(keys), std::move(jointIndex)));
			tracks.push_back(std::move(timeline));
		}
		
		if (!track.TranslateSamples.empty())
		{
			std::vector<TranslationKeyframe> keys;
			keys.reserve(track.TranslateSamples.size());
			
			for (auto & sample: track.TranslateSamples)
			{
				TranslationKeyframe key;
				key.TranslateX = sample.TranslateX;
				key.TranslateY = sample.TranslateY;
				key.Time = sample.Time;
				keys.push_back(std::move(key));
			}

			std::sort(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<TranslationKeyframe>);
			std::unique_ptr<TranslationTrack> timeline(new TranslationTrack(std::move(keys), std::move(jointIndex)));
			tracks.push_back(std::move(timeline));
		}
	}
	
	return AnimationClip(std::move(tracks));
}

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog
