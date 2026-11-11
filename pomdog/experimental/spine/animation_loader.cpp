// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/spine/animation_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/experimental/skeletal2d/animation_clip.h"
#include "pomdog/experimental/skeletal2d/joint_index.h"
#include "pomdog/experimental/skeletal2d/tracks/animation_track.h"
#include "pomdog/experimental/skeletal2d/tracks/rotation_track.h"
#include "pomdog/experimental/skeletal2d/tracks/scale_track.h"
#include "pomdog/experimental/skeletal2d/tracks/sprite_animation_track.h"
#include "pomdog/experimental/skeletal2d/tracks/translation_track.h"
#include "pomdog/experimental/spine/animation_clip_data.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/utility/string_hash32.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/skeletal2d_clip_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::spine {
namespace {

using skeletal2d::AnimationClip;
using skeletal2d::AnimationTimeInterval;
using skeletal2d::AnimationTrack;
using skeletal2d::JointIndex;
using skeletal2d::detail::RotationKeyframe;
using skeletal2d::detail::RotationTrack;
using skeletal2d::detail::ScaleKeyframe;
using skeletal2d::detail::ScaleTrack;
using skeletal2d::detail::SpriteAnimationTrack;
using skeletal2d::detail::SpriteKeyframe;
using skeletal2d::detail::TranslationKeyframe;
using skeletal2d::detail::TranslationTrack;

// NOTE: AnimationClipData has verified the tracks: every track has keys in
// ascending order of time, with values that the keyframes can represent.

void BuildJointAnimationTracks(
    const pomdogschemas::Skeletal2DClip& clip,
    std::vector<std::unique_ptr<AnimationTrack>>& tracks)
{
    if (const auto* rotationTracks = clip.rotation_tracks(); rotationTracks != nullptr) {
        for (const auto* track : *rotationTracks) {
            std::vector<RotationKeyframe> keys;
            keys.reserve(track->keys()->size());

            for (const auto* sample : *track->keys()) {
                RotationKeyframe key;
                key.Rotation = sample->rotation();
                key.Time = AnimationTimeInterval{sample->time()};
                keys.push_back(std::move(key));
            }

            auto timeline = std::make_unique<RotationTrack>(std::move(keys), JointIndex{track->joint()});
            tracks.push_back(std::move(timeline));
        }
    }

    if (const auto* scaleTracks = clip.scale_tracks(); scaleTracks != nullptr) {
        for (const auto* track : *scaleTracks) {
            std::vector<ScaleKeyframe> keys;
            keys.reserve(track->keys()->size());

            for (const auto* sample : *track->keys()) {
                ScaleKeyframe key;
                key.Scale = sample->scale();
                key.Time = AnimationTimeInterval{sample->time()};
                keys.push_back(std::move(key));
            }

            auto timeline = std::make_unique<ScaleTrack>(std::move(keys), JointIndex{track->joint()});
            tracks.push_back(std::move(timeline));
        }
    }

    if (const auto* translationTracks = clip.translation_tracks(); translationTracks != nullptr) {
        for (const auto* track : *translationTracks) {
            std::vector<TranslationKeyframe> keys;
            keys.reserve(track->keys()->size());

            for (const auto* sample : *track->keys()) {
                TranslationKeyframe key;
                key.TranslateX = sample->translate_x();
                key.TranslateY = sample->translate_y();
                key.Time = AnimationTimeInterval{sample->time()};
                keys.push_back(std::move(key));
            }

            auto timeline = std::make_unique<TranslationTrack>(std::move(keys), JointIndex{track->joint()});
            tracks.push_back(std::move(timeline));
        }
    }
}

[[nodiscard]] std::unique_ptr<Error>
BuildSpriteAnimationTracks(
    const pomdogschemas::Skeletal2DClip& clip,
    const TextureAtlas& textureAtlas,
    std::vector<std::unique_ptr<AnimationTrack>>& tracks)
{
    const auto* attachmentTracks = clip.attachment_tracks();
    if (attachmentTracks == nullptr) {
        return nullptr;
    }

    for (const auto* track : *attachmentTracks) {
        std::vector<SpriteKeyframe> keys;
        keys.reserve(track->keys()->size());

        for (const auto* sample : *track->keys()) {
            // NOTE: The key of an attachment is also the key of its region in the texture atlas.
            auto optIdx = textureAtlas.findRegionByKey(sample->attachment_key());
            if (!optIdx.has_value()) {
                return errors::make(pomdog::format("cannot find attachment {:#010x} in the texture atlas", sample->attachment_key()));
            }

            auto atlasRegion = textureAtlas.getRegion(*optIdx);

            constexpr bool isTextureRegionRotated = false;

            SpriteKeyframe key;
            key.Time = AnimationTimeInterval{sample->time()};
            key.TexturePage = 0;
            key.TextureRotate = isTextureRegionRotated;
            key.Subrect = Rect2D{
                atlasRegion.subrectX,
                atlasRegion.subrectY,
                atlasRegion.subrectWidth,
                atlasRegion.subrectHeight,
            };
            const auto regionW = static_cast<f32>(atlasRegion.width);
            const auto regionH = static_cast<f32>(atlasRegion.height);
            const auto textureXOffset = static_cast<f32>(atlasRegion.xOffset);
            const auto textureYOffset = static_cast<f32>(atlasRegion.yOffset);
            key.Origin.x = (regionW / 2 - textureXOffset) / atlasRegion.subrectWidth;
            key.Origin.y = (regionH / 2 - textureYOffset) / atlasRegion.subrectHeight;

            if (isTextureRegionRotated) {
                std::swap(key.Subrect.width, key.Subrect.height);
                std::swap(key.Origin.x, key.Origin.y);
            }

            keys.push_back(std::move(key));
        }

        auto timeline = std::make_unique<SpriteAnimationTrack>(std::move(keys), track->slot_key());
        tracks.push_back(std::move(timeline));
    }

    return nullptr;
}

[[nodiscard]] std::tuple<std::shared_ptr<skeletal2d::AnimationClip>, std::unique_ptr<Error>>
BuildAnimationClip(
    const pomdogschemas::Skeletal2DClip& clip,
    const std::shared_ptr<TextureAtlas>& textureAtlas)
{
    std::vector<std::unique_ptr<AnimationTrack>> tracks;
    BuildJointAnimationTracks(clip, tracks);

    if (textureAtlas != nullptr) {
        if (auto err = BuildSpriteAnimationTracks(clip, *textureAtlas, tracks); err != nullptr) {
            return std::make_tuple(nullptr, std::move(err));
        }
    }

    auto animationClip = std::make_shared<skeletal2d::AnimationClip>(std::move(tracks));
    return std::make_tuple(std::move(animationClip), nullptr);
}

} // namespace

std::tuple<std::shared_ptr<skeletal2d::AnimationClip>, std::unique_ptr<Error>>
CreateAnimationClip(
    const AnimationClipData& clipData,
    const std::shared_ptr<TextureAtlas>& textureAtlas,
    std::string_view name)
{
    const auto* clip = clipData.findClip(computeStringHash32(name));
    if (clip == nullptr) {
        auto err = errors::make(pomdog::format("cannot find animation clip '{}'", name));
        return std::make_tuple(nullptr, std::move(err));
    }

    auto [animationClip, err] = BuildAnimationClip(*clip, textureAtlas);
    if (err != nullptr) {
        auto wrapped = errors::wrap(std::move(err), pomdog::format("failed to create animation clip '{}'", name));
        return std::make_tuple(nullptr, std::move(wrapped));
    }
    return std::make_tuple(std::move(animationClip), nullptr);
}

std::tuple<std::shared_ptr<skeletal2d::AnimationClip>, std::unique_ptr<Error>>
CreateAnimationClipByKey(
    const AnimationClipData& clipData,
    const std::shared_ptr<TextureAtlas>& textureAtlas,
    u32 clipKey)
{
    const auto* clip = clipData.findClip(clipKey);
    if (clip == nullptr) {
        auto err = errors::make(pomdog::format("cannot find animation clip {:#010x}", clipKey));
        return std::make_tuple(nullptr, std::move(err));
    }

    auto [animationClip, err] = BuildAnimationClip(*clip, textureAtlas);
    if (err != nullptr) {
        auto wrapped = errors::wrap(std::move(err), pomdog::format("failed to create animation clip {:#010x}", clipKey));
        return std::make_tuple(nullptr, std::move(wrapped));
    }
    return std::make_tuple(std::move(animationClip), nullptr);
}

} // namespace pomdog::spine
