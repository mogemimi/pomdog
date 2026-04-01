// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/audio/wav.h"
#include "pomdog/audio/details/audio_clip_file.h"
#include "pomdog/audio/details/audio_clip_helpers.h"
#include "pomdog/content/audio/audio_container.h"
#include "pomdog/content/utility/binary_reader.h"
#include "pomdog/content/utility/make_fourcc.h"
#include "pomdog/memory/memcpy_span.h"
#include "pomdog/memory/memset_span.h"
#include "pomdog/utility/assert.h"
#include "pomdog/vfs/file.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

namespace BinaryReader = pomdog::detail::BinaryReader;
using pomdog::detail::makeFourCC;

struct SubChunkHeader final {
    u32 chunkID;
    u32 chunkSize;
};

struct PCMWaveFormat final {
    u16 formatTag;
    u16 channels;
    u32 samplesPerSec;
    u32 avgBytesPerSec;
    u16 blockAlign;
    u16 bitsPerSample;
};

class AudioClipFileWAV final : public detail::AudioClipFile {
private:
    std::vector<u8> buffer_ = {};
    std::span<const u8> waveData_ = {};
    std::span<const u8> waveSubspan_ = {};
    i32 totalSamples_ = 0;
    i32 sampleRate_ = 0;
    u16 bitsPerSample_ = 0;
    AudioChannels channels_ = AudioChannels::Mono;

public:
    AudioClipFileWAV() noexcept = default;

    AudioClipFileWAV(const AudioClipFileWAV&) = delete;
    AudioClipFileWAV& operator=(const AudioClipFileWAV&) = delete;

    ~AudioClipFileWAV() override
    {
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

        constexpr std::size_t maxFileSize = 1ULL * 1024 * 1024 * 1024; // 1 GB
        if (fileInfo.size > maxFileSize) {
            return errors::make("the wav file is too large");
        }

        waveData_ = {};
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

        std::span<const u8> buf{buffer_};

        // NOTE: Read RIFF chunk header.
        SubChunkHeader riffChunkHeader;
        if (auto err = BinaryReader::readAs(buf, riffChunkHeader.chunkID); err != nullptr) {
            return errors::wrap(std::move(err), "BinaryReader::readAs() failed");
        }
        if (auto err = BinaryReader::readAs(buf, riffChunkHeader.chunkSize); err != nullptr) {
            return errors::wrap(std::move(err), "BinaryReader::readAs() failed");
        }
        if (riffChunkHeader.chunkID != makeFourCC('R', 'I', 'F', 'F')) {
            return errors::make("missing RIFF chunk");
        }

        // NOTE: Read RIFF chunk.
        if (auto [fourCCType, subspan, err] = BinaryReader::readAs<u32>(buf); err != nullptr) {
            return errors::wrap(std::move(err), "BinaryReader::readAs() failed");
        }
        else if (fourCCType != makeFourCC('W', 'A', 'V', 'E')) {
            return errors::make("missing WAVE chunk");
        }
        else {
            buf = subspan;
        }

        // NOTE: Read fmt chunk header.
        SubChunkHeader fmtChunkHeader;
        if (auto err = BinaryReader::readAs(buf, fmtChunkHeader.chunkID); err != nullptr) {
            return errors::wrap(std::move(err), "BinaryReader::readAs() failed");
        }
        if (auto err = BinaryReader::readAs(buf, fmtChunkHeader.chunkSize); err != nullptr) {
            return errors::wrap(std::move(err), "BinaryReader::readAs() failed");
        }

        PCMWaveFormat waveFormat;

        constexpr std::size_t pcmWaveFormatSize =
            sizeof(waveFormat.formatTag) +
            sizeof(waveFormat.channels) +
            sizeof(waveFormat.samplesPerSec) +
            sizeof(waveFormat.avgBytesPerSec) +
            sizeof(waveFormat.blockAlign) +
            sizeof(waveFormat.bitsPerSample);

        if (fmtChunkHeader.chunkID != makeFourCC('f', 'm', 't', ' ')) {
            return errors::make("missing 'fmt ' chunk");
        }
        if (fmtChunkHeader.chunkSize < pcmWaveFormatSize) {
            return errors::make("'fmt ' chunk size is too small");
        }

        // NOTE: Read fmt chunk.
        if (auto err = BinaryReader::readAs(buf, waveFormat.formatTag); err != nullptr) {
            return errors::wrap(std::move(err), "BinaryReader::readAs() failed");
        }
        if (auto err = BinaryReader::readAs(buf, waveFormat.channels); err != nullptr) {
            return errors::wrap(std::move(err), "BinaryReader::readAs() failed");
        }
        if (auto err = BinaryReader::readAs(buf, waveFormat.samplesPerSec); err != nullptr) {
            return errors::wrap(std::move(err), "BinaryReader::readAs() failed");
        }
        if (auto err = BinaryReader::readAs(buf, waveFormat.avgBytesPerSec); err != nullptr) {
            return errors::wrap(std::move(err), "BinaryReader::readAs() failed");
        }
        if (auto err = BinaryReader::readAs(buf, waveFormat.blockAlign); err != nullptr) {
            return errors::wrap(std::move(err), "BinaryReader::readAs() failed");
        }
        if (auto err = BinaryReader::readAs(buf, waveFormat.bitsPerSample); err != nullptr) {
            return errors::wrap(std::move(err), "BinaryReader::readAs() failed");
        }

        // NOTE: WAVE_FORMAT_PCM
        constexpr u16 WaveFormatTagPCM = 0x0001;

        if (const bool hasExtraData = (waveFormat.formatTag != WaveFormatTagPCM); hasExtraData) {
            u16 extraDataByteSize = 0;
            if (auto err = BinaryReader::readAs(buf, extraDataByteSize); err != nullptr) {
                return errors::wrap(std::move(err), "BinaryReader::readAs() failed");
            }

            // NOTE: Skip extra data.
            if (auto err = BinaryReader::seek(buf, extraDataByteSize); err != nullptr) {
                return errors::wrap(std::move(err), "BinaryReader::seek() failed");
            }
        }

        // NOTE: Read wave audio data chunk header.
        SubChunkHeader waveDataChunkHeader;
        if (auto err = BinaryReader::readAs(buf, waveDataChunkHeader.chunkID); err != nullptr) {
            return errors::wrap(std::move(err), "BinaryReader::readAs() failed");
        }
        if (auto err = BinaryReader::readAs(buf, waveDataChunkHeader.chunkSize); err != nullptr) {
            return errors::wrap(std::move(err), "BinaryReader::readAs() failed");
        }

        if (waveDataChunkHeader.chunkID != makeFourCC('d', 'a', 't', 'a')) {
            return errors::make("missing 'data' chunk");
        }
        if (waveDataChunkHeader.chunkSize <= 0) {
            return errors::make("data chunk is empty");
        }
        if (waveDataChunkHeader.chunkSize > buf.size()) {
            return errors::make("data chunk size must be <= file.size()");
        }
        const auto waveData = buf.subspan(0, waveDataChunkHeader.chunkSize);

        waveData_ = waveData;
        bitsPerSample_ = waveFormat.bitsPerSample;
        sampleRate_ = static_cast<i32>(waveFormat.samplesPerSec);

        if (auto c = detail::toAudioChannels(waveFormat.channels); c == std::nullopt) {
            return errors::make("invalid waveFormat.channels");
        }
        else {
            channels_ = *c;
        }

        totalSamples_ = detail::calculateSampleCount(
            static_cast<i32>(waveData_.size()),
            bitsPerSample_,
            channels_);

        waveSubspan_ = waveData_;

        return nullptr;
    }

    /// Gets the length of the audio clip in samples.
    [[nodiscard]] i32
    getSampleCount() const noexcept override
    {
        return totalSamples_;
    }

    /// Gets the number of samples per second.
    [[nodiscard]] i32
    getSampleRate() const noexcept override
    {
        return sampleRate_;
    }

    /// Gets the number of bits per sample.
    [[nodiscard]] u16
    getBitsPerSample() const noexcept override
    {
        return bitsPerSample_;
    }

    /// Gets the number of channels in the audio clip.
    [[nodiscard]] AudioChannels
    getChannels() const noexcept override
    {
        return channels_;
    }

    [[nodiscard]] std::unique_ptr<Error>
    seekStart() override
    {
        waveSubspan_ = waveData_;
        return nullptr;
    }

    POMDOG_MSVC_SUPPRESS_WARNING_PUSH
    POMDOG_MSVC_SUPPRESS_WARNING(5045) // NOTE: Spectre mitigation
    [[nodiscard]] std::tuple<i32, std::unique_ptr<Error>>
    readSamplesInterleaved(std::span<u8> outBuffer) override
    {
        if (outBuffer.empty()) {
            return std::make_tuple(0, errors::make("invalid buffer size"));
        }

        const auto channelCount = detail::toAudioChannelCount(channels_);
        const auto bytesPerSample = static_cast<u32>(channelCount * (bitsPerSample_ / 8));

        const auto minReadSize = std::min(outBuffer.size(), waveSubspan_.size());

        const auto sampleCount = minReadSize / bytesPerSample;
        const auto readSize = std::min(bytesPerSample * sampleCount, minReadSize);

        if (readSize > 0) {
            memcpySpan(outBuffer, std::span<const u8>{waveSubspan_.data(), readSize});
        }

        waveSubspan_ = waveSubspan_.subspan(readSize);

        return std::make_tuple(static_cast<i32>(sampleCount), nullptr);
    }
    POMDOG_MSVC_SUPPRESS_WARNING_POP
};

} // namespace

[[nodiscard]] std::tuple<AudioContainer, std::unique_ptr<Error>>
decodeWAV(std::span<const u8> file) noexcept
{
    AudioContainer container;

    const auto make_result = [&](std::unique_ptr<Error>&& err) -> std::tuple<AudioContainer, std::unique_ptr<Error>> {
        return std::make_tuple(std::move(container), std::move(err));
    };

    // NOTE: Read RIFF chunk header.
    SubChunkHeader riffChunkHeader;
    if (auto err = BinaryReader::readAs(file, riffChunkHeader.chunkID); err != nullptr) {
        return make_result(errors::wrap(std::move(err), "BinaryReader::readAs() failed"));
    }
    if (auto err = BinaryReader::readAs(file, riffChunkHeader.chunkSize); err != nullptr) {
        return make_result(errors::wrap(std::move(err), "BinaryReader::readAs() failed"));
    }
    if (riffChunkHeader.chunkID != makeFourCC('R', 'I', 'F', 'F')) {
        return make_result(errors::make("missing RIFF chunk"));
    }

    // NOTE: Read RIFF chunk.
    if (auto [fourCCType, subspan, err] = BinaryReader::readAs<u32>(file); err != nullptr) {
        return make_result(errors::wrap(std::move(err), "BinaryReader::readAs() failed"));
    }
    else if (fourCCType != makeFourCC('W', 'A', 'V', 'E')) {
        return make_result(errors::make("missing WAVE chunk"));
    }
    else {
        file = subspan;
    }

    // NOTE: Read fmt chunk header.
    SubChunkHeader fmtChunkHeader;
    if (auto err = BinaryReader::readAs(file, fmtChunkHeader.chunkID); err != nullptr) {
        return make_result(errors::wrap(std::move(err), "BinaryReader::readAs() failed"));
    }
    if (auto err = BinaryReader::readAs(file, fmtChunkHeader.chunkSize); err != nullptr) {
        return make_result(errors::wrap(std::move(err), "BinaryReader::readAs() failed"));
    }

    PCMWaveFormat waveFormat;

    constexpr std::size_t pcmWaveFormatSize =
        sizeof(waveFormat.formatTag) +
        sizeof(waveFormat.channels) +
        sizeof(waveFormat.samplesPerSec) +
        sizeof(waveFormat.avgBytesPerSec) +
        sizeof(waveFormat.blockAlign) +
        sizeof(waveFormat.bitsPerSample);

    if (fmtChunkHeader.chunkID != makeFourCC('f', 'm', 't', ' ')) {
        return make_result(errors::make("missing 'fmt ' chunk"));
    }
    if (fmtChunkHeader.chunkSize < pcmWaveFormatSize) {
        return make_result(errors::make("'fmt ' chunk size is too small"));
    }

    // NOTE: Read fmt chunk.
    if (auto err = BinaryReader::readAs(file, waveFormat.formatTag); err != nullptr) {
        return make_result(errors::wrap(std::move(err), "BinaryReader::readAs() failed"));
    }
    if (auto err = BinaryReader::readAs(file, waveFormat.channels); err != nullptr) {
        return make_result(errors::wrap(std::move(err), "BinaryReader::readAs() failed"));
    }
    if (auto err = BinaryReader::readAs(file, waveFormat.samplesPerSec); err != nullptr) {
        return make_result(errors::wrap(std::move(err), "BinaryReader::readAs() failed"));
    }
    if (auto err = BinaryReader::readAs(file, waveFormat.avgBytesPerSec); err != nullptr) {
        return make_result(errors::wrap(std::move(err), "BinaryReader::readAs() failed"));
    }
    if (auto err = BinaryReader::readAs(file, waveFormat.blockAlign); err != nullptr) {
        return make_result(errors::wrap(std::move(err), "BinaryReader::readAs() failed"));
    }
    if (auto err = BinaryReader::readAs(file, waveFormat.bitsPerSample); err != nullptr) {
        return make_result(errors::wrap(std::move(err), "BinaryReader::readAs() failed"));
    }

    // NOTE: WAVE_FORMAT_PCM
    constexpr u16 WaveFormatTagPCM = 0x0001;

    if (const bool hasExtraData = (waveFormat.formatTag != WaveFormatTagPCM); hasExtraData) {
        u16 extraDataByteSize = 0;
        if (auto err = BinaryReader::readAs(file, extraDataByteSize); err != nullptr) {
            return make_result(errors::wrap(std::move(err), "BinaryReader::readAs() failed"));
        }

        // NOTE: Skip extra data.
        if (auto err = BinaryReader::seek(file, extraDataByteSize); err != nullptr) {
            return make_result(errors::wrap(std::move(err), "BinaryReader::seek() failed"));
        }
    }

    // NOTE: Read wave audio data chunk header.
    SubChunkHeader waveDataChunkHeader;
    if (auto err = BinaryReader::readAs(file, waveDataChunkHeader.chunkID); err != nullptr) {
        return make_result(errors::wrap(std::move(err), "BinaryReader::readAs() failed"));
    }
    if (auto err = BinaryReader::readAs(file, waveDataChunkHeader.chunkSize); err != nullptr) {
        return make_result(errors::wrap(std::move(err), "BinaryReader::readAs() failed"));
    }

    if (waveDataChunkHeader.chunkID != makeFourCC('d', 'a', 't', 'a')) {
        return make_result(errors::make("missing 'data' chunk"));
    }
    if (waveDataChunkHeader.chunkSize <= 0) {
        return make_result(errors::make("data chunk is empty"));
    }
    if (waveDataChunkHeader.chunkSize > file.size()) {
        return make_result(errors::make("data chunk size must be <= file.size()"));
    }
    const auto waveData = file.subspan(0, waveDataChunkHeader.chunkSize);

    container.audioData = waveData;
    container.bitsPerSample = static_cast<i32>(waveFormat.bitsPerSample);
    container.sampleRate = static_cast<i32>(waveFormat.samplesPerSec);

    if (auto c = detail::toAudioChannels(waveFormat.channels); c == std::nullopt) {
        return make_result(errors::make("invalid waveFormat.channels"));
    }
    else {
        container.channels = *c;
    }

    return std::make_tuple(std::move(container), nullptr);
}

[[nodiscard]] std::tuple<std::unique_ptr<detail::AudioClipFile>, std::unique_ptr<Error>>
openAudioClipFileWAV(std::shared_ptr<vfs::File> file) noexcept
{
    auto clip = std::make_unique<AudioClipFileWAV>();

    if (auto err = clip->open(std::move(file)); err != nullptr) {
        return std::make_tuple(std::move(clip), errors::wrap(std::move(err), "failed to open WAV file"));
    }

    return std::make_tuple(std::move(clip), nullptr);
}

} // namespace pomdog
