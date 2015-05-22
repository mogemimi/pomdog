// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "SpriteAnimationLoader.hpp"
#include "SkeletonDesc.hpp"
#include "Pomdog.Experimental/Skeletal2D/SpriteAnimationTrack.hpp"
#include "Pomdog.Experimental/Skeletal2D/detail/AnimationKeyHelper.hpp"
#include "Pomdog/Utility/detail/CRC32.hpp"
#include <utility>
#include <algorithm>

namespace Pomdog {
namespace Spine {
//-----------------------------------------------------------------------
std::vector<Detail::Skeletal2D::SpriteAnimationTrack> CreateSpriteAnimationTrack(
    SkeletonDesc const& desc,
    TexturePacker::TextureAtlas const& textureAtlas,
    char const* name)
{
    using namespace Detail::Skeletal2D;
    using Detail::CRC32;

    auto iter = std::find_if(std::begin(desc.AnimationClips), std::end(desc.AnimationClips),
        [name](AnimationClipDesc const& clip){ return clip.Name == name; });

    POMDOG_ASSERT(std::end(desc.AnimationClips) != iter);

    if (std::end(desc.AnimationClips) == iter) {
        ///@todo Not implemented
        // Error: Cannot find animation clip
        return {};
    }

    auto & animationClip = *iter;

    std::vector<SpriteAnimationTrack> tracks;

    for (auto & animationTrack: animationClip.SlotTracks)
    {
        auto slotIndex = CRC32::ComputeCRC32(animationTrack.SlotName);

        if (!animationTrack.AttachmentSamples.empty())
        {
            std::vector<SpriteKeyframe> keys;
            keys.reserve(animationTrack.AttachmentSamples.size());

            for (auto & sample: animationTrack.AttachmentSamples)
            {
                auto textureAtlasRegion = std::find_if(std::begin(textureAtlas.regions), std::end(textureAtlas.regions),
                    [&](TexturePacker::TextureAtlasRegion const& region) {
                        return region.Name == sample.AttachmentName;
                    });

                if (textureAtlasRegion == std::end(textureAtlas.regions)) {
                    ///@todo Not implemented
                    // Error: Cannot find attachment
                    continue;
                }

                SpriteKeyframe key;
                key.Time = sample.Time;
                key.TexturePage = textureAtlasRegion->TexturePage;
                key.TextureRotate = textureAtlasRegion->Region.Rotate;
                key.Subrect = textureAtlasRegion->Region.Subrect;
                auto textureXOffset = textureAtlasRegion->Region.XOffset;
                auto textureYOffset = textureAtlasRegion->Region.YOffset;
                key.Origin.X = static_cast<float>(textureAtlasRegion->Region.Width / 2 - textureXOffset)
                    / textureAtlasRegion->Region.Subrect.Width;
                key.Origin.Y = static_cast<float>(textureAtlasRegion->Region.Height / 2 - textureYOffset)
                    / textureAtlasRegion->Region.Subrect.Height;

                if (textureAtlasRegion->Region.Rotate) {
                    std::swap(key.Subrect.Width, key.Subrect.Height);
                    std::swap(key.Origin.X, key.Origin.Y);
                }

                keys.push_back(std::move(key));
            }

            std::sort(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<SpriteKeyframe>);
            tracks.emplace_back(std::move(keys), slotIndex);
        }
    }

    return tracks;
}

}// namespace Spine
}// namespace Pomdog
