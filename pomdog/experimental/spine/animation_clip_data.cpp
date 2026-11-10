// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/spine/animation_clip_data.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/skeletal2d_clip_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
#include <iterator>
#include <string>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::spine {
namespace {

// NOTE: A joint index of 255 means no joint.
constexpr u8 noJoint = 255;

// NOTE: schemas/skeletal2d_clip.fbs limits the key values to these ranges,
// which the keyframes of the runtime tracks can represent.
constexpr f32 maxKeyRotation = math::Pi<f32>;
constexpr f32 maxKeyScale = 20.0f;
constexpr f32 maxKeyTranslation = 1024.0f;

template <typename T>
[[nodiscard]] u32
getSize(const flatbuffers::Vector<T>* vector) noexcept
{
    if (vector == nullptr) {
        return 0;
    }
    return vector->size();
}

/// Returns true if `value` is within [-limit, limit]. Returns false for NaN.
[[nodiscard]] bool
isWithin(f32 value, f32 limit) noexcept
{
    return (value >= -limit) && (value <= limit);
}

[[nodiscard]] bool
isValidKey(const pomdogschemas::Skeletal2DRotationKey& key) noexcept
{
    return isWithin(key.rotation(), maxKeyRotation);
}

[[nodiscard]] bool
isValidKey(const pomdogschemas::Skeletal2DScaleKey& key) noexcept
{
    return isWithin(key.scale(), maxKeyScale);
}

[[nodiscard]] bool
isValidKey(const pomdogschemas::Skeletal2DTranslationKey& key) noexcept
{
    return isWithin(key.translate_x(), maxKeyTranslation) && isWithin(key.translate_y(), maxKeyTranslation);
}

[[nodiscard]] bool
isValidKey([[maybe_unused]] const pomdogschemas::Skeletal2DAttachmentKey& key) noexcept
{
    return true;
}

/// Verifies that a track has keys in ascending order of time with valid values.
template <typename Key>
[[nodiscard]] std::unique_ptr<Error>
validateKeys(const flatbuffers::Vector<const Key*>* keys) noexcept
{
    if (getSize(keys) == 0) {
        return errors::make("track has no keys");
    }

    f32 previousTime = 0.0f;
    for (const auto* key : *keys) {
        const auto time = key->time();
        if (!std::isfinite(time) || time < previousTime) {
            return errors::make("keys must be in ascending order of time, which is never negative");
        }
        previousTime = time;

        if (!isValidKey(*key)) {
            return errors::make("track has a key whose value is out of range");
        }
    }
    return nullptr;
}

template <typename Track>
[[nodiscard]] std::unique_ptr<Error>
validateJointTracks(
    const flatbuffers::Vector<flatbuffers::Offset<Track>>* tracks,
    std::string_view kind) noexcept
{
    const auto trackCount = getSize(tracks);
    for (u32 index = 0; index < trackCount; ++index) {
        const auto* track = tracks->Get(index);
        if (track->joint() == noJoint) {
            return errors::make(pomdog::format("{} track {} animates no joint", kind, index));
        }
        if (auto err = validateKeys(track->keys()); err != nullptr) {
            return errors::wrap(std::move(err), pomdog::format("invalid {} track {}", kind, index));
        }
    }
    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
validateClip(const pomdogschemas::Skeletal2DClip& clip) noexcept
{
    if (auto err = validateJointTracks(clip.rotation_tracks(), "rotation"); err != nullptr) {
        return err;
    }
    if (auto err = validateJointTracks(clip.scale_tracks(), "scale"); err != nullptr) {
        return err;
    }
    if (auto err = validateJointTracks(clip.translation_tracks(), "translation"); err != nullptr) {
        return err;
    }

    const auto* attachmentTracks = clip.attachment_tracks();
    const auto trackCount = getSize(attachmentTracks);
    for (u32 index = 0; index < trackCount; ++index) {
        if (auto err = validateKeys(attachmentTracks->Get(index)->keys()); err != nullptr) {
            return errors::wrap(std::move(err), pomdog::format("invalid attachment track {}", index));
        }
    }
    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
validateClipSet(const pomdogschemas::Skeletal2DClipSet& table) noexcept
{
    const auto* keys = table.keys();
    const auto* clips = table.clips();
    const auto clipCount = getSize(clips);
    if (getSize(keys) != clipCount) {
        return errors::make("keys and clips must have the same length");
    }

    for (u32 index = 0; index < clipCount; ++index) {
        // NOTE: findClip() searches the keys with a binary search.
        if (index > 0 && keys->Get(index - 1) >= keys->Get(index)) {
            return errors::make("keys must be in ascending order without duplicates");
        }
        if (auto err = validateClip(*clips->Get(index)); err != nullptr) {
            return errors::wrap(std::move(err), pomdog::format("invalid clip {}", index));
        }
    }
    return nullptr;
}

} // namespace

AnimationClipData::AnimationClipData(
    std::shared_ptr<vfs::File> file,
    const pomdogschemas::Skeletal2DClipSet* table) noexcept
    : file_(std::move(file))
    , table_(table)
{
    POMDOG_ASSERT(file_ != nullptr);
    POMDOG_ASSERT(table_ != nullptr);
}

const pomdogschemas::Skeletal2DClip*
AnimationClipData::findClip(u32 key) const noexcept
{
    POMDOG_ASSERT(table_ != nullptr);

    const auto* keys = table_->keys();
    if (keys == nullptr) {
        return nullptr;
    }

    const auto iter = std::lower_bound(keys->begin(), keys->end(), key);
    if (iter == keys->end() || *iter != key) {
        return nullptr;
    }

    const auto index = std::distance(keys->begin(), iter);
    return table_->clips()->Get(static_cast<u32>(index));
}

std::tuple<std::shared_ptr<AnimationClipData>, std::unique_ptr<Error>>
loadAnimationClipData(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    std::string_view filePath) noexcept
{
    const auto path = std::string{filePath};

    auto [file, openErr] = vfs::open(fs, path);
    if (openErr != nullptr) {
        return {nullptr, errors::wrap(std::move(openErr), "cannot open clip file, " + path)};
    }

    auto [bytes, viewErr] = file->view();
    if (viewErr != nullptr) {
        return {nullptr, errors::wrap(std::move(viewErr), "cannot read clip file, " + path)};
    }

    flatbuffers::Verifier verifier(bytes.data(), bytes.size());
    if (!pomdogschemas::VerifySkeletal2DClipSetBuffer(verifier)) {
        return {nullptr, errors::make("FlatBuffers verification failed for clip file, " + path)};
    }

    const auto* table = pomdogschemas::GetSkeletal2DClipSet(bytes.data());
    if (auto err = validateClipSet(*table); err != nullptr) {
        return {nullptr, errors::wrap(std::move(err), "invalid clip file, " + path)};
    }

    return {std::make_shared<AnimationClipData>(std::move(file), table), nullptr};
}

} // namespace pomdog::spine
