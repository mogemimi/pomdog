// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "MSWaveAudioLoader.hpp"

#include "Pomdog/Basic/Platform.hpp"
#if defined(POMDOG_PLATFORM_MACOSX) \
    || defined(POMDOG_PLATFORM_APPLE_IOS) \
    || defined(POMDOG_PLATFORM_LINUX)
#include "../../SoundSystem.OpenAL/AudioClipAL.hpp"
#elif defined(POMDOG_PLATFORM_WIN32) \
    || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "../../SoundSystem.XAudio2/AudioClipXAudio2.hpp"
#endif

#include "Pomdog/Audio/AudioClip.hpp"
#include "Pomdog/Content/Utility/BinaryReader.hpp"
#include "Pomdog/Content/Utility/MakeFourCC.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

#if 0
#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
#include <AudioToolbox/AudioConverter.h>
#include <AudioToolbox/AudioFile.h>
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "Pomdog/Content/Utility/MakeFourCC.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include <mmsystem.h>
#endif
#endif

#include <memory>
#include <utility>
#include <vector>

namespace Pomdog {
namespace Detail {
namespace {

AudioChannels ToAudioChannels(std::uint32_t channels) noexcept
{
    POMDOG_ASSERT(channels > 0);
    POMDOG_ASSERT(channels <= 2);

    switch (channels) {
    case 1: return AudioChannels::Mono;
    case 2: return AudioChannels::Stereo;
    default:
        break;
    }
    return AudioChannels::Mono;
}
#if 0
#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)

std::unique_ptr<AudioClip> LoadMSWave_Apple(const std::string& filePath)
{
    CFStringRef filename = CFStringCreateWithCString(nullptr, filePath.c_str(), kCFStringEncodingUTF8);
    CFURLRef url = CFURLCreateWithFileSystemPath(nullptr, filename, kCFURLPOSIXPathStyle, false);
    CFRelease(filename);

    AudioFileID audioFile;
    auto errorCode = AudioFileOpenURL(url, kAudioFileReadPermission, kAudioFileWAVEType, &audioFile);
    CFRelease(url);

    if (errorCode != noErr)
    {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Cannot open audio file");
    }

    AudioStreamBasicDescription basicDescription;
    UInt32 propertySize = sizeof(basicDescription);
    errorCode = AudioFileGetProperty(audioFile, kAudioFilePropertyDataFormat, &propertySize, &basicDescription);

    if (errorCode != noErr)
    {
        AudioFileClose(audioFile);
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to get audio file propety");
    }

    if (basicDescription.mFormatID != kAudioFormatLinearPCM)
    {
        AudioFileClose(audioFile);
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "This audio file is not linear PCM format");
    }

    UInt64 audioDataByteCount = 0;
    propertySize = sizeof(audioDataByteCount);
    errorCode = AudioFileGetProperty(audioFile, kAudioFilePropertyAudioDataByteCount, &propertySize, &audioDataByteCount);

    if (errorCode != noErr)
    {
        AudioFileClose(audioFile);
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Cannot get the byte count of the audio data");
    }

    Float64 estimatedDuration = 0;
    propertySize = sizeof(estimatedDuration);
    errorCode = AudioFileGetProperty(audioFile, kAudioFilePropertyEstimatedDuration, &propertySize, &estimatedDuration);

    if (errorCode != noErr)
    {
        AudioFileClose(audioFile);
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Cannot get the estimated duration of the audio data");
    }

    if (basicDescription.mChannelsPerFrame < 1
        && basicDescription.mChannelsPerFrame > 2)
    {
        AudioFileClose(audioFile);
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "4, 5.1, 6.1 and 7.1 channel audio are not supported. You can use mono and stereo.");
    }

    if (basicDescription.mBitsPerChannel < 8
        && basicDescription.mBitsPerChannel > 32
        && (basicDescription.mBitsPerChannel % 8 != 0))
    {
        AudioFileClose(audioFile);
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "4, 5.1, 6.1 and 7.1 channel audio are not supported. You can use mono and stereo.");
    }

    POMDOG_ASSERT(basicDescription.mBitsPerChannel == 8
        || basicDescription.mBitsPerChannel == 16
        || basicDescription.mBitsPerChannel == 24
        || basicDescription.mBitsPerChannel == 32);

    std::vector<std::uint8_t> audioData;
    audioData.resize(audioDataByteCount);

    UInt32 byteCountToRead = static_cast<UInt32>(audioData.size());
    errorCode = AudioFileReadBytes(audioFile, false, 0, &byteCountToRead, audioData.data());
    AudioFileClose(audioFile);

    if (errorCode != noErr)
    {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to read audio data");
    }

    if (byteCountToRead != audioDataByteCount)
    {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to read audio data");
    }

    using Detail::SoundSystem::OpenAL::AudioClipAL;

    auto channels = ToAudioChannels(basicDescription.mChannelsPerFrame);

    auto nativeAudioClip = std::make_unique<AudioClipAL>(
        audioData.data(), audioData.size(),
        basicDescription.mSampleRate,
        basicDescription.mBitsPerChannel,
        channels);

    auto audioClip = std::make_unique<AudioClip>(
        std::move(nativeAudioClip),
        basicDescription.mSampleRate,
        basicDescription.mBitsPerChannel,
        channels);

    return std::move(audioClip);
}

#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)

MMCKINFO ReadRiffChunk(HMMIO ioHandle)
{
    POMDOG_ASSERT(ioHandle);

    MMCKINFO riffChunk;

    auto mmResult = ::mmioDescend(ioHandle, &riffChunk, nullptr, 0);
    if (MMSYSERR_NOERROR != mmResult) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Cannot read Multi-Media I/O wave file, mmResult=" + std::to_string(mmResult));
    }

    if (MakeFourCC('R', 'I', 'F', 'F') != riffChunk.ckid) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Cannot read wave file. this file is a invalid wave file");
    }

    if (MakeFourCC('W', 'A', 'V', 'E') != riffChunk.fccType) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Cannot read wave file. this file is a invalid wave file");
    }

    return std::move(riffChunk);
}

std::vector<std::uint8_t> ReadWaveFormat(HMMIO ioHandle, const MMCKINFO& riffChunk)
{
    POMDOG_ASSERT(ioHandle);

    MMCKINFO chunkInfo;
    chunkInfo.ckid = MakeFourCC('f', 'm', 't', ' ');

    auto mmResult = ::mmioDescend(ioHandle, &chunkInfo, &riffChunk, MMIO_FINDCHUNK);
    if (MMSYSERR_NOERROR != mmResult) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to search the input file for the 'fmt ' chunk, mmResult=" + std::to_string(mmResult));
    }

    if (chunkInfo.cksize < sizeof(PCMWAVEFORMAT)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "chunkInfo.cksize=" + std::to_string(chunkInfo.cksize));
    }

    PCMWAVEFORMAT pcmWaveFormat;

    auto byteSize = ::mmioRead(ioHandle, reinterpret_cast<HPSTR>(&pcmWaveFormat), sizeof(pcmWaveFormat));
    if (byteSize != sizeof(pcmWaveFormat)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to Read the 'fmt ' chunk into <pcmWaveFormat>");
    }

    std::vector<std::uint8_t> waveFormat;

    if (WAVE_FORMAT_PCM == pcmWaveFormat.wf.wFormatTag)
    {
        waveFormat.resize(sizeof(WAVEFORMATEX));

        static_assert(sizeof(WAVEFORMATEX) >= sizeof(pcmWaveFormat), "");
        POMDOG_ASSERT(waveFormat.size() >= sizeof(pcmWaveFormat));
        std::memcpy(waveFormat.data(), &pcmWaveFormat, sizeof(pcmWaveFormat));

        reinterpret_cast<WAVEFORMATEX*>(waveFormat.data())->cbSize = 0;
    }
    else
    {
        WORD extraBytes = 0;
        byteSize = ::mmioRead(ioHandle, reinterpret_cast<CHAR*>(&extraBytes), sizeof(WORD));
        if (byteSize != sizeof(WORD)) {
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to Read the extraBytes");
        }

        waveFormat.resize(sizeof(WAVEFORMATEX) + extraBytes);

        static_assert(sizeof(WAVEFORMATEX) >= sizeof(pcmWaveFormat), "");
        POMDOG_ASSERT(waveFormat.size() >= sizeof(pcmWaveFormat));
        std::memcpy(waveFormat.data(), &pcmWaveFormat, sizeof(pcmWaveFormat));

        reinterpret_cast<WAVEFORMATEX*>(waveFormat.data())->cbSize = extraBytes;

        byteSize = ::mmioRead(ioHandle, reinterpret_cast<CHAR*>(waveFormat.data() + sizeof(WAVEFORMATEX)), extraBytes);
        if (byteSize != extraBytes) {
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to read the extra data");
        }
    }

    mmResult = ::mmioAscend(ioHandle, &chunkInfo, 0);
    if (MMSYSERR_NOERROR != mmResult) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to ascend the input file out of the 'fmt ' chunk, mmResult=" + std::to_string(mmResult));
    }

    return std::move(waveFormat);
}

MMCKINFO SeekDataChunk(HMMIO ioHandle, const MMCKINFO& riffChunk)
{
    POMDOG_ASSERT(ioHandle);

    constexpr LONG seekErrorCode = -1;
    if (seekErrorCode == ::mmioSeek(ioHandle, riffChunk.dwDataOffset + sizeof(FOURCC), SEEK_SET)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to call mmioSeek");
    }

    MMCKINFO dataChunk;
    dataChunk.ckid = MakeFourCC('d', 'a', 't', 'a');

    auto mmResult = ::mmioDescend(ioHandle, &dataChunk, &riffChunk, MMIO_FINDCHUNK);
    if (MMSYSERR_NOERROR != mmResult) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to call mmioDescend, mmResult=" + std::to_string(mmResult));
    }
    return std::move(dataChunk);
}

std::vector<std::uint8_t> ReadWaveAudioData(HMMIO ioHandle, const MMCKINFO& dataChunk)
{
    POMDOG_ASSERT(ioHandle);

    MMIOINFO mmioInfoIn;
    if (0 != ::mmioGetInfo(ioHandle, &mmioInfoIn, 0)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Buffer size is zero");
    }

    if (dataChunk.cksize <= 0) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Data chunk is empty");
    }

    std::vector<std::uint8_t> result;
    result.resize(dataChunk.cksize);

    for (DWORD index = 0; index < dataChunk.cksize; ++index)
    {
        if (mmioInfoIn.pchNext == mmioInfoIn.pchEndRead)
        {
            auto mmResult = ::mmioAdvance(ioHandle, &mmioInfoIn, MMIO_READ);
            if (MMSYSERR_NOERROR != mmResult) {
                ///@todo Not implemeneted
                // error: FUS RO DAH!
                break;
            }

            if (mmioInfoIn.pchNext == mmioInfoIn.pchEndRead) {
                ///@todo Not implemeneted
                // error: FUS RO DAH!
                break;
            }
        }

        *(result.data() + index) = *(reinterpret_cast<::BYTE*>(mmioInfoIn.pchNext));
        ++mmioInfoIn.pchNext;
    }

    if (0 != ::mmioSetInfo(ioHandle, &mmioInfoIn, 0)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to call mmioSetInfo");
    }

    return std::move(result);
}

std::unique_ptr<AudioClip> LoadMSWave_Win32(const std::string& filePath)
{
    HMMIO ioHandle = ::mmioOpen(const_cast<LPSTR>(filePath.c_str()), nullptr, MMIO_ALLOCBUF | MMIO_READ);

    if (!ioHandle) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Cannot open Multi-Media I/O wave file");
    }

    try {
        auto riffChunk = ReadRiffChunk(ioHandle);
        auto waveFormat = ReadWaveFormat(ioHandle, riffChunk);
        auto dataChunk = SeekDataChunk(ioHandle, riffChunk);
        auto audioData = ReadWaveAudioData(ioHandle, dataChunk);

        ::mmioClose(ioHandle, 0);
        ioHandle = nullptr;

        using Detail::SoundSystem::XAudio2::AudioClipXAudio2;
        auto nativeAudioClip = std::make_unique<AudioClipXAudio2>(
            std::move(audioData), std::move(waveFormat));

        auto format = nativeAudioClip->WaveFormat();
        POMDOG_ASSERT(format);

        auto audioClip = std::make_unique<AudioClip>(std::move(nativeAudioClip),
            format->nSamplesPerSec, format->wBitsPerSample, ToAudioChannels(format->nChannels));

        return std::move(audioClip);
    }
    catch (const std::exception& e) {
        if (ioHandle) {
            ::mmioClose(ioHandle, 0);
            ioHandle = nullptr;
        }
        throw e;
    }
}
#endif
#endif

struct RiffChunk {
    std::uint32_t ChunkId;
    std::uint32_t ChunkSize;
    std::uint32_t FourCCType;
};

struct SubChunkHeader {
    std::uint32_t ChunkId;
    std::uint32_t ChunkSize;
};

struct PcmWaveFormat {
    std::uint16_t FormatTag;
    std::uint16_t Channels;
    std::uint32_t SamplesPerSec;
    std::uint32_t AvgBytesPerSec;
    std::uint16_t BlockAlign;
    std::uint16_t BitsPerSample;
};

struct WaveFormat {
    Pomdog::Detail::PcmWaveFormat PcmWaveFormat;
    std::uint16_t ExtraBytes;
    std::vector<std::uint8_t> ExtraData;
};

namespace WaveFormatTags {
    // WAVE_FORMAT_PCM
    static constexpr std::uint16_t PCM = 0x0001;
}

RiffChunk ReadRiffChunk(std::ifstream & stream)
{
    POMDOG_ASSERT(stream);
    auto riffChunk = BinaryReader::Read<RiffChunk>(stream);

    if (stream.fail()) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Cannot read 'RIFF' chunk descriptor form wave file");
    }
    if (MakeFourCC('R', 'I', 'F', 'F') != riffChunk.ChunkId) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Cannot read wave file. this file is a invalid wave file");
    }
    if (MakeFourCC('W', 'A', 'V', 'E') != riffChunk.FourCCType) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Cannot read wave file. this file is a invalid wave file");
    }
    return riffChunk;
}

WaveFormat ReadWaveFormat(std::ifstream & stream)
{
    POMDOG_ASSERT(stream);

    const auto fmtChunkHeader = BinaryReader::Read<SubChunkHeader>(stream);

    if (stream.fail()) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Cannot read 'fmt ' chunk header form wave file");
    }
    if (MakeFourCC('f', 'm', 't', ' ') != fmtChunkHeader.ChunkId) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to search the wave file for the 'fmt ' chunk");
    }
    if (fmtChunkHeader.ChunkSize < sizeof(PcmWaveFormat)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "The 'fmt ' chunk size is too small: " + std::to_string(fmtChunkHeader.ChunkSize));
    }

    WaveFormat waveFormat;
    waveFormat.PcmWaveFormat = BinaryReader::Read<PcmWaveFormat>(stream);

    if (stream.fail()) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to Read the 'fmt ' chunk into pcmWaveFormat");
    }

    if (WaveFormatTags::PCM == waveFormat.PcmWaveFormat.FormatTag) {
        waveFormat.ExtraBytes = 0;
        waveFormat.ExtraData.clear();
    }
    else {
        waveFormat.ExtraBytes = BinaryReader::Read<std::uint16_t>(stream);
        if (stream.fail()) {
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to Read the extraBytes");
        }

        waveFormat.ExtraData.resize(waveFormat.ExtraBytes);
        stream.read(reinterpret_cast<char*>(waveFormat.ExtraData.data()), waveFormat.ExtraBytes);

        if (stream.fail()) {
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to read the extra data");
        }
    }

    return waveFormat;
}

std::vector<std::uint8_t> ReadWaveAudioData(std::ifstream & stream)
{
    POMDOG_ASSERT(stream);

    const auto chunkHeader = BinaryReader::Read<SubChunkHeader>(stream);

    if (stream.fail()) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Cannot read 'data' chunk header form wave file");
    }
    if (MakeFourCC('d', 'a', 't', 'a') != chunkHeader.ChunkId) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to search the wave file for the 'data' chunk");
    }
    if (chunkHeader.ChunkSize <= 0) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Data chunk is empty");
    }

    std::vector<std::uint8_t> audioData;
    audioData.resize(chunkHeader.ChunkSize);

    for (auto & dest : audioData) {
        dest = BinaryReader::Read<std::uint8_t>(stream);
        if (stream.fail()) {
            break;
        }
    }

    if (!stream) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to read wave data");
    }
    return audioData;
}

} // unnamed namespace

std::unique_ptr<AudioClip> MSWaveAudioLoader::Load(BinaryFileStream && binaryFile)
{
    POMDOG_ASSERT(binaryFile.Stream);
    POMDOG_ASSERT(binaryFile.SizeInBytes > 8);

    constexpr auto MinimumWaveFormatSizeInBytes = 4 * 11;
    if (binaryFile.SizeInBytes < MinimumWaveFormatSizeInBytes) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "This wave file is too small.");
    }

    auto & stream = binaryFile.Stream;

    stream.clear();
    stream.seekg(0, std::ios_base::beg);

    POMDOG_ASSERT(stream);

    try {
        ReadRiffChunk(stream);
        auto waveFormat = ReadWaveFormat(stream);
        auto audioData = ReadWaveAudioData(stream);

        auto channels = ToAudioChannels(waveFormat.PcmWaveFormat.Channels);

#if defined(POMDOG_PLATFORM_MACOSX) \
    || defined(POMDOG_PLATFORM_APPLE_IOS) \
    || defined(POMDOG_PLATFORM_LINUX)
        using Detail::SoundSystem::OpenAL::AudioClipAL;
        auto nativeAudioClip = std::make_unique<AudioClipAL>(
            audioData.data(),
            audioData.size(),
            waveFormat.PcmWaveFormat.SamplesPerSec,
            waveFormat.PcmWaveFormat.BitsPerSample,
            channels);

        auto audioClip = std::make_unique<AudioClip>(
            std::move(nativeAudioClip),
            waveFormat.PcmWaveFormat.SamplesPerSec,
            waveFormat.PcmWaveFormat.BitsPerSample,
            channels);

        return audioClip;

#elif defined(POMDOG_PLATFORM_WIN32) \
    || defined(POMDOG_PLATFORM_XBOX_ONE)
        std::vector<std::uint8_t> waveFormatData;
        waveFormatData.resize(sizeof(waveFormat.PcmWaveFormat)
            + sizeof(waveFormat.ExtraBytes)
            + waveFormat.ExtraBytes);

        auto offset = sizeof(waveFormat.PcmWaveFormat) + sizeof(waveFormat.ExtraBytes);
        std::memcpy(waveFormatData.data(), &waveFormat, offset);
        std::memcpy(waveFormatData.data() + offset, waveFormat.ExtraData.data(), waveFormat.ExtraBytes);

        using Detail::SoundSystem::XAudio2::AudioClipXAudio2;
        auto nativeAudioClip = std::make_unique<AudioClipXAudio2>(
            std::move(audioData),
            std::move(waveFormatData));

        auto audioClip = std::make_unique<AudioClip>(
            std::move(nativeAudioClip),
            waveFormat.PcmWaveFormat.SamplesPerSec,
            waveFormat.PcmWaveFormat.BitsPerSample,
            channels);

        return std::move(audioClip);
#endif
    }
    catch (const std::exception& e) {
        if (stream.is_open()) {
            stream.close();
        }
#if defined(DEBUG) && !defined(NDEBUG)
        Log::Warning("Failed to read audio file.\n", e.what());
#endif
        throw e;
    }
}

} // namespace Detail
} // namespace Pomdog
