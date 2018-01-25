// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "LogSkeletalInfo.hpp"
#include <Pomdog/Pomdog.hpp>

namespace TestApp {

using namespace Pomdog;

void LogTexturePackerInfo(TexturePacker::TextureAtlas const& textureAtlas)
{
    Log::Info(StringHelper::Format("TextureAtlas.Pages = %ld", textureAtlas.pages.size()));
    Log::Info(StringHelper::Format("TextureAtlas.Regions = %ld", textureAtlas.regions.size()));

    for (auto page: textureAtlas.pages)
    {
        Log::Info(StringHelper::Format("Page.Name = %s", page.Name.c_str()));
    }

    for (auto region: textureAtlas.regions)
    {
        Log::Info(StringHelper::Format("Region.Name = %s", region.Name.c_str()));
        Log::Info(StringHelper::Format("  Region.Page = %d", region.TexturePage));
        Log::Info(StringHelper::Format("  Region.Rotate = %d", region.Region.Rotate?1:0));
        Log::Info(StringHelper::Format("  Region.XY = %d, %d", (int)region.Region.Subrect.X, (int)region.Region.Subrect.Y));
        Log::Info(StringHelper::Format("  Region.Size = %d, %d", (int)region.Region.Subrect.Width, (int)region.Region.Subrect.Height));
        Log::Info(StringHelper::Format("  Region.Offset = %d %d", (int)region.Region.XOffset, (int)region.Region.YOffset));
        Log::Info(StringHelper::Format("  Region.OriginalSize = %d %d", (int)region.Region.Width, (int)region.Region.Height));
    }

    Log::Info("-------------------------");
}

void LogSkeletalInfo(Spine::SkeletonDesc const& skeletonDesc)
{
    int boneIndex = 0;
    for (auto & bone: skeletonDesc.Bones)
    {
        Log::Info(StringHelper::Format("MaidChan/%s", bone.Name.c_str()));
        Log::Info(StringHelper::Format("  Bone.Index = %d", boneIndex));
        Log::Info(StringHelper::Format("  Bone.Translate = %f, %f", bone.Pose.Translate.X, bone.Pose.Translate.Y));
        Log::Info(StringHelper::Format("  Bone.Rotation = %f", bone.Pose.Rotation.value));
        Log::Info(StringHelper::Format("  Bone.Scale = %f", bone.Pose.Scale));
        ++boneIndex;
    }

    Log::Info("-------------------------");
    for (auto & animationClip: skeletonDesc.AnimationClips)
    {
        for (auto & track: animationClip.BoneTracks)
        {
            Log::Info(StringHelper::Format("AnimationClip/%s/Bones/%s", animationClip.Name.c_str(), track.BoneName.c_str()));
            for (auto & sample: track.TranslateSamples)
            {
                Log::Info(StringHelper::Format("  {time: %f, x: %f, y: %f},",
                    sample.Time.count(),
                    sample.TranslateX.ToFloat(),
                    sample.TranslateY.ToFloat()));
            }
            for (auto & sample: track.RotateSamples)
            {
                Log::Info(StringHelper::Format("  {time: %f, rotate: %f},",
                    sample.Time.count(),
                    sample.Rotation.ToFloat()));
            }
            for (auto & sample: track.ScaleSamples)
            {
                Log::Info(StringHelper::Format("  {time: %f, rotate: %f},",
                    sample.Time.count(),
                    sample.Scale.ToFloat()));
            }
        }
        for (auto & track: animationClip.SlotTracks)
        {
            Log::Info(StringHelper::Format("AnimationClip/%s/Slots/%s", animationClip.Name.c_str(), track.SlotName.c_str()));
            for (auto & sample: track.AttachmentSamples)
            {
                Log::Info(StringHelper::Format("  {time: %f, name: %s},",
                    sample.Time.count(),
                    sample.AttachmentName.c_str()));
            }
        }
    }
}

}// namespace TestApp
