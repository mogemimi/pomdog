// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/audio/vorbis.h"
#include "pomdog/audio/audio_channels.h"
#include "pomdog/audio/details/audio_clip_file.h"
#include "pomdog/audio/details/audio_clip_helpers.h"
#include "pomdog/content/audio/audio_container.h"
#include "pomdog/content/utility/make_fourcc.h"
#include "pomdog/memory/memset_span.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/scope_guard.h"
#include "pomdog/vfs/file.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <stb/stb_vorbis.h>
#include <limits>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

class AudioClipFileOggVorbis final : public detail::AudioClipFile {
private:
    std::vector<u8> buffer_ = {};
    ::stb_vorbis* vorbis_ = nullptr;
    i32 samplesPerChannel_ = 0;
    i32 sampleRate_ = 0;
    u16 bitsPerSample_ = 0;
    AudioChannels channels_ = AudioChannels::Mono;

public:
    AudioClipFileOggVorbis() noexcept = default;

    AudioClipFileOggVorbis(const AudioClipFileOggVorbis&) = delete;
    AudioClipFileOggVorbis& operator=(const AudioClipFileOggVorbis&) = delete;

    ~AudioClipFileOggVorbis() override
    {
        if (vorbis_ != nullptr) {
            ::stb_vorbis_close(vorbis_);
            vorbis_ = nullptr;
        }
    }

    [[nodiscard]] std::unique_ptr<Error>
    open(std::shared_ptr<vfs::File> file)
    {
        if (file == nullptr) {
            return errors::make("file must be != nullptr");
        }

        auto [fileInfo, statErr] = file->stat();
        if (statErr != nullptr) {
            return errors::wrap(std::move(statErr), "failed to get file info");
        }

        // NOTE: stb_vorbis_open_memory takes int for size, so limit to INT_MAX.
        constexpr auto maxFileSize = static_cast<std::size_t>(std::numeric_limits<int>::max());
        if (fileInfo.size > maxFileSize) {
            return errors::make("the ogg vorbis file is too large");
        }

        buffer_.clear();
        buffer_.resize(fileInfo.size);

        constexpr bool debugFillMemory = true;
        if constexpr (debugFillMemory) {
            memsetSpan(std::span{buffer_}, u8(0xAC));
        }

        auto [bytesRead, readErr] = file->read(buffer_);
        if (readErr != nullptr) {
            return errors::wrap(std::move(readErr), "failed to read file");
        }

        int openError = 0;
        vorbis_ = ::stb_vorbis_open_memory(buffer_.data(), static_cast<int>(buffer_.size()), &openError, nullptr);
        if (openError != 0) {
            return errors::make("failed to open Ogg Vorbis file");
        }

        const auto fail = [&] {
            if (vorbis_ != nullptr) {
                ::stb_vorbis_close(vorbis_);
                vorbis_ = nullptr;
            }
        };

        const auto info = ::stb_vorbis_get_info(vorbis_);
        if (info.channels <= 0) {
            fail();
            return errors::make("info.channels must be > 0");
        }

        if (auto c = detail::toAudioChannels(info.channels); c == std::nullopt) {
            fail();
            return errors::make("invalid info.channels");
        }
        else {
            channels_ = *c;
        }

        samplesPerChannel_ = static_cast<i32>(::stb_vorbis_stream_length_in_samples(vorbis_));
        if (samplesPerChannel_ <= 0) {
            fail();
            return errors::make("samplesPerChannel_ must be > 0");
        }

        sampleRate_ = static_cast<i32>(info.sample_rate);
        bitsPerSample_ = u16(16);

        return nullptr;
    }

    [[nodiscard]] i32
    getSampleCount() const noexcept override
    {
        return samplesPerChannel_;
    }

    [[nodiscard]] i32
    getSampleRate() const noexcept override
    {
        return sampleRate_;
    }

    [[nodiscard]] u16
    getBitsPerSample() const noexcept override
    {
        return bitsPerSample_;
    }

    [[nodiscard]] AudioChannels
    getChannels() const noexcept override
    {
        return channels_;
    }

    [[nodiscard]] std::unique_ptr<Error>
    seekStart() override
    {
        if (vorbis_ == nullptr) {
            return errors::make("vorbis_ is null");
        }
        ::stb_vorbis_seek_start(vorbis_);
        return nullptr;
    }

    [[nodiscard]] std::tuple<i32, std::unique_ptr<Error>>
    readSamplesInterleaved(std::span<u8> outBuffer) override
    {
        if (vorbis_ == nullptr) {
            return std::make_tuple(0, errors::make("vorbis_ is null"));
        }

        if (bitsPerSample_ != 16) {
            return std::make_tuple(0, errors::make("not implemented yet"));
        }

        const auto channelCount = detail::toAudioChannelCount(channels_);
        const auto bytesPerSample = static_cast<u32>(bitsPerSample_ / 8);

        // NOTE: `requestSamples` is "samples per N-channels", not "samples per channel"
        auto requestSamples = static_cast<i32>(outBuffer.size() / bytesPerSample);

        if (requestSamples <= 0) {
            return std::make_tuple(0, errors::make("invalid buffer size"));
        }

        const auto streamSamples = samplesPerChannel_ * static_cast<i32>(channelCount);

        const auto offset = ::stb_vorbis_get_sample_offset(vorbis_);
        POMDOG_ASSERT(offset <= streamSamples);

        if (const auto remain = streamSamples - offset; remain < requestSamples) {
            requestSamples = remain;
        }

        // NOTE: `samplesCount` is "samples per channel" not "samples per N-channels"
        const auto samplesCount = ::stb_vorbis_get_samples_short_interleaved(
            vorbis_,
            channelCount,
            reinterpret_cast<i16*>(outBuffer.data()),
            requestSamples);

        return std::make_tuple(samplesCount, nullptr);
    }
};

} // namespace

POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(5045) // NOTE: Spectre mitigation
[[nodiscard]] std::tuple<AudioContainer, std::unique_ptr<Error>>
decodeOggVorbis(std::span<const u8> file) noexcept
{
    AudioContainer container;

    const auto make_result = [&](std::unique_ptr<Error>&& err) -> std::tuple<AudioContainer, std::unique_ptr<Error>> {
        return std::make_tuple(std::move(container), std::move(err));
    };

    // NOTE: stb_vorbis_open_memory takes int for size, so limit to INT_MAX.
    constexpr auto maxFileSize = static_cast<std::size_t>(std::numeric_limits<int>::max());
    if (file.size() > maxFileSize) {
        return make_result(errors::make("the ogg vorbis data is too large"));
    }

    int openError = 0;
    auto vorbis = ::stb_vorbis_open_memory(file.data(), static_cast<int>(file.size()), &openError, nullptr);

    if (openError != 0) {
        return make_result(errors::make("failed to open Ogg Vorbis file"));
    }

    pomdog::ScopeGuard defer([&] {
        ::stb_vorbis_close(vorbis);
    });

    const auto info = ::stb_vorbis_get_info(vorbis);
    if (info.channels <= 0) {
        return make_result(errors::make("info.channels must be > 0"));
    }

    if (auto c = detail::toAudioChannels(info.channels); c == std::nullopt) {
        return make_result(errors::make("invalid info.channels"));
    }
    else {
        container.channels = *c;
    }

    const auto samplesPerChannel = static_cast<i32>(::stb_vorbis_stream_length_in_samples(vorbis));
    if (samplesPerChannel <= 0) {
        return make_result(errors::make("samplesPerChannel must be > 0"));
    }

    constexpr u16 bytesPerSample = sizeof(i16);
    constexpr u16 bitsPerSample = bytesPerSample * 8;

    std::vector<u8> rawData;
    rawData.clear();
    rawData.resize(
        bytesPerSample *
        static_cast<std::size_t>(samplesPerChannel) *
        static_cast<std::size_t>(info.channels));

    const auto sampleCount = ::stb_vorbis_get_samples_short_interleaved(
        vorbis,
        info.channels,
        reinterpret_cast<i16*>(rawData.data()),
        samplesPerChannel * info.channels);

    if (sampleCount < 0) {
        return make_result(errors::make("failed to decode Ogg Vorbis file"));
    }

    if (sampleCount < samplesPerChannel) {
        rawData.resize(
            sizeof(i16) *
            static_cast<std::size_t>(sampleCount) *
            static_cast<std::size_t>(info.channels));
        rawData.shrink_to_fit();
    }

    container.rawData = std::move(rawData);
    container.audioData = std::span<const u8>{container.rawData.data(), container.rawData.size()};
    container.bitsPerSample = bitsPerSample;
    container.sampleRate = static_cast<i32>(info.sample_rate);

    return std::make_tuple(std::move(container), nullptr);
}
POMDOG_MSVC_SUPPRESS_WARNING_POP

[[nodiscard]] std::tuple<std::unique_ptr<detail::AudioClipFile>, std::unique_ptr<Error>>
openAudioClipFileOggVorbis(std::shared_ptr<vfs::File> file) noexcept
{
    auto clip = std::make_unique<AudioClipFileOggVorbis>();

    if (auto err = clip->open(std::move(file)); err != nullptr) {
        return std::make_tuple(std::move(clip), errors::wrap(std::move(err), "failed to open Ogg Vorbis file"));
    }

    return std::make_tuple(std::move(clip), nullptr);
}

} // namespace pomdog
