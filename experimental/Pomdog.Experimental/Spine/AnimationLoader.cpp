// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "AnimationLoader.hpp"
#include "Pomdog.Experimental/Spine/SkeletonDesc.hpp"
#include "Pomdog.Experimental/Skeletal2D/AnimationClip.hpp"
#include "Pomdog.Experimental/Skeletal2D/AnimationTrack.hpp"
#include "Pomdog.Experimental/Skeletal2D/RotationTrack.hpp"
#include "Pomdog.Experimental/Skeletal2D/ScaleTrack.hpp"
#include "Pomdog.Experimental/Skeletal2D/TranslationTrack.hpp"
#include "Pomdog.Experimental/Skeletal2D/JointIndex.hpp"
#include "Pomdog.Experimental/Skeletal2D/detail/AnimationKeyHelper.hpp"
#include <utility>
#include <algorithm>

namespace Pomdog {
namespace Spine {
namespace {

JointIndex FindJoint(std::vector<BoneDesc> const& bones, std::string const& name)
{
    auto iter = std::find_if(std::begin(bones), std::end(bones),
        [&name](BoneDesc const& desc){ return desc.Name == name; });

    if (iter != std::end(bones)) {
        return JointIndex(std::distance(std::begin(bones), iter));
    }
    return {};
}

} // unnamed namespace
//-----------------------------------------------------------------------
AnimationClip CreateAnimationClip(SkeletonDesc const& desc, std::string const& name)
{
    auto iter = std::find_if(std::begin(desc.AnimationClips), std::end(desc.AnimationClips),
        [&name](AnimationClipDesc const& clip){ return clip.Name == name; });

    POMDOG_ASSERT(std::end(desc.AnimationClips) != iter);

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

        using namespace Pomdog::Detail::Skeletal2D;

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
            auto timeline = std::make_unique<RotationTrack>(std::move(keys), std::move(jointIndex));
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
            auto timeline = std::make_unique<ScaleTrack>(std::move(keys), std::move(jointIndex));
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
            auto timeline = std::make_unique<TranslationTrack>(std::move(keys), std::move(jointIndex));
            tracks.push_back(std::move(timeline));
        }
    }

    return AnimationClip(std::move(tracks));
}

} // namespace Spine
} // namespace Pomdog
