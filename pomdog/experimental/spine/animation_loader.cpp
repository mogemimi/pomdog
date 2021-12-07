// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/spine/animation_loader.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/skeletal2d/animation_clip.hpp"
#include "pomdog/experimental/skeletal2d/joint_index.hpp"
#include "pomdog/experimental/skeletal2d/tracks/animation_key_helper.hpp"
#include "pomdog/experimental/skeletal2d/tracks/animation_track.hpp"
#include "pomdog/experimental/skeletal2d/tracks/rotation_track.hpp"
#include "pomdog/experimental/skeletal2d/tracks/scale_track.hpp"
#include "pomdog/experimental/skeletal2d/tracks/sprite_animation_track.hpp"
#include "pomdog/experimental/skeletal2d/tracks/translation_track.hpp"
#include "pomdog/experimental/spine/skeleton_desc.hpp"
#include "pomdog/utility/crc32.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Spine {
namespace {

using Skeletal2D::AnimationClip;
using Skeletal2D::AnimationTrack;
using Skeletal2D::JointIndex;
using Skeletal2D::Detail::SpriteAnimationTrack;
using Skeletal2D::Detail::SpriteKeyframe;

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
        auto slotIndex = Detail::CRC32::ComputeCRC32(animationTrack.SlotName);

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
            key.TextureRotate = textureAtlasRegion->Region.Rotate;
            key.Subrect = textureAtlasRegion->Region.Subrect;
            const auto regionW = static_cast<float>(textureAtlasRegion->Region.Width);
            const auto regionH = static_cast<float>(textureAtlasRegion->Region.Height);
            const auto textureXOffset = static_cast<float>(textureAtlasRegion->Region.XOffset);
            const auto textureYOffset = static_cast<float>(textureAtlasRegion->Region.YOffset);
            key.Origin.X = (regionW / 2 - textureXOffset) / textureAtlasRegion->Region.Subrect.Width;
            key.Origin.Y = (regionH / 2 - textureYOffset) / textureAtlasRegion->Region.Subrect.Height;

            if (textureAtlasRegion->Region.Rotate) {
                std::swap(key.Subrect.Width, key.Subrect.Height);
                std::swap(key.Origin.X, key.Origin.Y);
            }

            keys.push_back(std::move(key));
        }

        using Skeletal2D::Detail::AnimationKeyHelper::Less;
        std::sort(std::begin(keys), std::end(keys), Less<SpriteKeyframe>);

        auto timeline = std::make_unique<SpriteAnimationTrack>(std::move(keys), std::move(slotIndex));
        tracks.push_back(std::move(timeline));
    }
}

} // namespace

std::tuple<std::shared_ptr<Skeletal2D::AnimationClip>, std::unique_ptr<Error>>
CreateAnimationClip(
    const SkeletonDesc& desc,
    const std::optional<TexturePacker::TextureAtlas>& textureAtlas,
    const std::string& name)
{
    auto iter = std::find_if(std::begin(desc.AnimationClips), std::end(desc.AnimationClips),
        [&name](const AnimationClipDesc& clip) { return clip.Name == name; });

    if (std::end(desc.AnimationClips) == iter) {
        auto err = Errors::New("cannot find animation clip '" + name + "'");
        return std::make_tuple(nullptr, std::move(err));
    }

    auto& animationClip = *iter;

    std::vector<std::unique_ptr<AnimationTrack>> tracks;

    for (auto& track : animationClip.BoneTracks) {
        auto jointIndex = FindJoint(desc.Bones, track.BoneName);
        POMDOG_ASSERT(jointIndex);

        using namespace Pomdog::Skeletal2D::Detail;

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

    auto clip = std::make_shared<Skeletal2D::AnimationClip>(std::move(tracks));
    return std::make_tuple(std::move(clip), nullptr);
}

} // namespace Pomdog::Spine
