//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "MSWaveAudioLoader.hpp"

#include "Pomdog/Config/Platform.hpp"
#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
#include <AudioToolbox/AudioFile.h>
#include <AudioToolbox/AudioConverter.h>
#endif

namespace Pomdog {
namespace Details {

AudioClipSource MSWaveAudioLoader::Load(std::string const& filePath)
{
	CFStringRef filename = CFStringCreateWithCString(nullptr, filePath.c_str(), kCFStringEncodingUTF8);
	CFURLRef url = CFURLCreateWithFileSystemPath(nullptr, filename, kCFURLPOSIXPathStyle, false);
	CFRelease(filename);

	AudioFileID audioFile;
	auto errorCode = AudioFileOpenURL(url, kAudioFileReadPermission, kAudioFileWAVEType, &audioFile);
	CFRelease(url);
	
	if (errorCode != noErr)
	{
		///@todo Not implemeneted
		// Error
		return {};
	}
	
	AudioStreamBasicDescription basicDescription;
	UInt32 propertySize = sizeof(basicDescription);
	errorCode = AudioFileGetProperty(audioFile, kAudioFilePropertyDataFormat, &propertySize, &basicDescription);
	
	if (errorCode != noErr)
	{
		///@todo Not implemeneted
		// Error
		AudioFileClose(audioFile);
		return {};
	}
	
	if (basicDescription.mFormatID != kAudioFormatLinearPCM)
	{
		///@todo Not implemeneted
		// Error
		AudioFileClose(audioFile);
		return {};
	}

	UInt64 audioDataByteCount = 0;
	propertySize = sizeof(audioDataByteCount);
	errorCode = AudioFileGetProperty(audioFile, kAudioFilePropertyAudioDataByteCount, &propertySize, &audioDataByteCount);
	
	if (errorCode != noErr)
	{
		///@todo Not implemeneted
		// Error
		AudioFileClose(audioFile);
		return {};
	}
	
	Float64 estimatedDuration = 0;
	propertySize = sizeof(estimatedDuration);
	errorCode = AudioFileGetProperty(audioFile, kAudioFilePropertyEstimatedDuration, &propertySize, &estimatedDuration);

	if (errorCode != noErr)
	{
		///@todo Not implemeneted
		// Error
		AudioFileClose(audioFile);
		return {};
	}
	
	AudioClipSource audioClip;
	audioClip.SampleRate = basicDescription.mSampleRate;
	audioClip.Duration = DurationSeconds{estimatedDuration};
	
	if (basicDescription.mChannelsPerFrame == 1) {
		audioClip.Channels = AudioChannels::Mono;
	}
	else if (basicDescription.mChannelsPerFrame == 2) {
		audioClip.Channels = AudioChannels::Stereo;
	}
	else {
		///@todo Not implemeneted
		// Error: Not supported
		AudioFileClose(audioFile);
		return {};
	}
	
	if (basicDescription.mBitsPerChannel == 8) {
		audioClip.BitsPerSample = 8;
	}
	else if (basicDescription.mBitsPerChannel == 16) {
		audioClip.BitsPerSample = 16;
	}
	else {
		///@todo Not implemeneted
		// Error: Not supported
		AudioFileClose(audioFile);
		return {};
	}

	audioClip.Data.resize(audioDataByteCount);
	UInt32 byteCountToRead = static_cast<UInt32>(audioClip.Data.size());
	errorCode = AudioFileReadBytes(audioFile, false, 0, &byteCountToRead, audioClip.Data.data());
	AudioFileClose(audioFile);
	
	if (errorCode != noErr)
	{
		///@todo Not implemeneted
		// Error
		return {};
	}
	
	if (byteCountToRead != audioDataByteCount)
	{
		///@todo Not implemeneted
		// Error
		return {};
	}
		
	return std::move(audioClip);
}

}// namespace Details
}// namespace Pomdog
