//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_AUDIOCLIPAL_88AB8594_0292_40AB_84D9_F82EDDAD63A9_HPP
#define POMDOG_AUDIOCLIPAL_88AB8594_0292_40AB_84D9_F82EDDAD63A9_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "PrerequisitesOpenAL.hpp"
#include "../Utility/Noncopyable.hpp"
#include <Pomdog/Utility/Optional.hpp>
#include <cstdint>
#include <cstddef>

namespace Pomdog {

enum class AudioChannels: std::uint8_t;

namespace Details {
namespace SoundSystem {
namespace OpenAL {

class AudioClipAL final: Noncopyable {
public:
	AudioClipAL(void const* data, std::size_t size,
		std::uint32_t sampleRate, std::uint16_t bitsPerSample,
		AudioChannels channel);

	~AudioClipAL();

	std::size_t SizeInBytes() const;

	ALuint NativeBuffer() const;

private:
	Optional<ALuint> buffer;
	std::size_t sizeInBytes;
};

}// namespace OpenAL
}// namespace SoundSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_AUDIOCLIPAL_88AB8594_0292_40AB_84D9_F82EDDAD63A9_HPP)
