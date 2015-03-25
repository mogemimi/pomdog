// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "MSWaveAudioLoader.hpp"

#include "Pomdog/Audio/AudioClip.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Basic/Platform.hpp"
#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
#include "../../SoundSystem.OpenAL/AudioClipAL.hpp"
#include <AudioToolbox/AudioFile.h>
#include <AudioToolbox/AudioConverter.h>
#include <vector>
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "MakeFourCC.hpp"
#include "../../SoundSystem.XAudio2/AudioClipXAudio2.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include <mmsystem.h>
#endif
#include <memory>
#include <utility>

namespace Pomdog {
namespace Detail {
namespace {
//-----------------------------------------------------------------------
static AudioChannels ToAudioChannels(std::uint32_t channels)
{
	POMDOG_ASSERT(channels > 0);
	POMDOG_ASSERT(channels <= 2);

	switch (channels) {
	case 1:
		return AudioChannels::Mono;
	case 2:
		return AudioChannels::Stereo;
	default:
		break;
	}
	return AudioChannels::Mono;
}
#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
//-----------------------------------------------------------------------
static std::unique_ptr<AudioClip> LoadMSWave_Apple(std::string const& filePath)
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
//-----------------------------------------------------------------------
static MMCKINFO ReadRiffChunk(HMMIO ioHandle)
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
//-----------------------------------------------------------------------
static std::vector<std::uint8_t> ReadWaveFormat(HMMIO ioHandle, MMCKINFO const& riffChunk)
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
//-----------------------------------------------------------------------
static MMCKINFO SeekDataChunk(HMMIO ioHandle, MMCKINFO const& riffChunk)
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
//-----------------------------------------------------------------------
static std::vector<std::uint8_t> ReadWaveAudioData(HMMIO ioHandle, MMCKINFO const& dataChunk)
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
//-----------------------------------------------------------------------
static std::unique_ptr<AudioClip> LoadMSWave_Win32(std::string const& filePath)
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
	catch (std::exception const& e) {
		if (ioHandle) {
			::mmioClose(ioHandle, 0);
			ioHandle = nullptr;
		}
		throw e;
	}
}
#endif
}// unnamed namespace
 //-----------------------------------------------------------------------
std::unique_ptr<AudioClip> MSWaveAudioLoader::Load(std::string const& filePath)
{
#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
	return LoadMSWave_Apple(filePath);
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
	return LoadMSWave_Win32(filePath);
#else
#error "TODO: Not implemented"
#endif
}
//-----------------------------------------------------------------------
}// namespace Detail
}// namespace Pomdog
