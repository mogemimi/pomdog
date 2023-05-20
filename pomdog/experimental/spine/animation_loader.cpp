// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/spine/animation_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/skeletal2d/animation_clip.h"
#include "pomdog/experimental/skeletal2d/joint_index.h"
#include "pomdog/experimental/skeletal2d/tracks/animation_key_helper.h"
#include "pomdog/experimental/skeletal2d/tracks/animation_track.h"
#include "pomdog/experimental/skeletal2d/tracks/rotation_track.h"
#include "pomdog/experimental/skeletal2d/tracks/scale_track.h"
#include "pomdog/experimental/skeletal2d/tracks/sprite_animation_track.h"
#include "pomdog/experimental/skeletal2d/tracks/translation_track.h"
#include "pomdog/experimental/spine/skeleton_desc.h"
#include "pomdog/utility/crc32.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::spine {
namespace {

using skeletal2d::AnimationClip;
using skeletal2d::AnimationTrack;
using skeletal2d::JointIndex;
using skeletal2d::detail::SpriteAnimationTrack;
using skeletal2d::detail::SpriteKeyframe;

JointIndex
FindJoint(const std::vector<BoneDesc>& bones, const std::string& name)
{
    auto iter = std::find_if(std::begin(bones), std::end(bones),
        [&name](const BoneDesc& desc) { return desc.Name == name; });

    if (iter != std::end(bones)) {
        const auto dist = std::distance(std::begin(bones), iter);
        return JointIndex{static_cast<std::uint8_t>(dist)};
    }
    return {};
}

void BuildSpriteAnimationTracks(
    const AnimationClipDesc& animationClip,
    const TexturePacker::TextureAtlas& textureAtlas,
    std::vector<std::unique_ptr<AnimationTrack>>& tracks)
{
    for (auto& animationTrack : animationClip.SlotTracks) {
        auto slotIndex = detail::crc32::ComputeCRC32(animationTrack.SlotName);

        if (animationTrack.AttachmentSamples.empty()) {
            continue;
        }

        std::vector<SpriteKeyframe> keys;
        keys.reserve(animationTrack.AttachmentSamples.size());

        for (auto& sample : animationTrack.AttachmentSamples) {
            auto textureAtlasRegion = std::find_if(
                std::begin(textureAtlas.regions),
                std::end(textureAtlas.regions),
                [&](const TexturePacker::TextureAtlasRegion& region) {
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
            key.TextureRotate = textureAtlasRegion->Region.rotate;
            key.Subrect = textureAtlasRegion->Region.subrect;
            const auto regionW = static_cast<float>(textureAtlasRegion->Region.width);
            const auto regionH = static_cast<float>(textureAtlasRegion->Region.height);
            const auto textureXOffset = static_cast<float>(textureAtlasRegion->Region.xOffset);
            const auto textureYOffset = static_cast<float>(textureAtlasRegion->Region.yOffset);
            key.Origin.x = (regionW / 2 - textureXOffset) / textureAtlasRegion->Region.subrect.width;
            key.Origin.y = (regionH / 2 - textureYOffset) / textureAtlasRegion->Region.subrect.height;

            if (textureAtlasRegion->Region.rotate) {
                std::swap(key.Subrect.width, key.Subrect.height);
                std::swap(key.Origin.x, key.Origin.y);
            }

            keys.push_back(std::move(key));
        }

        using skeletal2d::detail::AnimationKeyHelper::Less;
        std::sort(std::begin(keys), std::end(keys), Less<SpriteKeyframe>);

        auto timeline = std::make_unique<SpriteAnimationTrack>(std::move(keys), std::move(slotIndex));
        tracks.push_back(std::move(timeline));
    }
}

} // namespace

std::tuple<std::shared_ptr<skeletal2d::AnimationClip>, std::unique_ptr<Error>>
CreateAnimationClip(
    const SkeletonDesc& desc,
    const std::optional<TexturePacker::TextureAtlas>& textureAtlas,
    const std::string& name)
{
    auto iter = std::find_if(std::begin(desc.AnimationClips), std::end(desc.AnimationClips),
        [&name](const AnimationClipDesc& clip) { return clip.Name == name; });

    if (std::end(desc.AnimationClips) == iter) {
        auto err = errors::New("cannot find animation clip '" + name + "'");
        return std::make_tuple(nullptr, std::move(err));
    }

    auto& animationClip = *iter;

    std::vector<std::unique_ptr<AnimationTrack>> tracks;

    for (auto& track : animationClip.BoneTracks) {
        auto jointIndex = FindJoint(desc.Bones, track.BoneName);
        POMDOG_ASSERT(jointIndex);

        using namespace pomdog::skeletal2d::detail;

        if (!track.RotateSamples.empty()) {
            std::vector<RotationKeyframe> keys;
            keys.reserve(track.RotateSamples.size());

            for (auto& sample : track.RotateSamples) {
                RotationKeyframe key;
                key.Rotation = sample.Rotation;
                key.Time = sample.Time;
                keys.push_back(std::move(key));
            }

            std::sort(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<RotationKeyframe>);
            auto timeline = std::make_unique<RotationTrack>(std::move(keys), std::move(jointIndex));
            tracks.push_back(std::move(timeline));
        }

        if (!track.ScaleSamples.empty()) {
            std::vector<ScaleKeyframe> keys;
            keys.reserve(track.ScaleSamples.size());

            for (auto& sample : track.ScaleSamples) {
                ScaleKeyframe key;
                key.Scale = sample.Scale;
                key.Time = sample.Time;
                keys.push_back(std::move(key));
            }

            std::sort(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<ScaleKeyframe>);
            auto timeline = std::make_unique<ScaleTrack>(std::move(keys), std::move(jointIndex));
            tracks.push_back(std::move(timeline));
        }

        if (!track.TranslateSamples.empty()) {
            std::vector<TranslationKeyframe> keys;
            keys.reserve(track.TranslateSamples.size());

            for (auto& sample : track.TranslateSamples) {
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

    if (textureAtlas != std::nullopt) {
        BuildSpriteAnimationTracks(animationClip, *textureAtlas, tracks);
    }

    auto clip = std::make_shared<skeletal2d::AnimationClip>(std::move(tracks));
    return std::make_tuple(std::move(clip), nullptr);
}

} // namespace pomdog::spine
