//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_AUDIOBUFFERAL_88AB8594_0292_40AB_84D9_F82EDDAD63A9_HPP
#define POMDOG_AUDIOBUFFERAL_88AB8594_0292_40AB_84D9_F82EDDAD63A9_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "PrerequisitesOpenAL.hpp"
#include "../Utility/Noncopyable.hpp"
#include <Pomdog/Utility/Optional.hpp>
#include <cstdint>

namespace Pomdog {

enum class AudioChannels: std::uint8_t;

namespace Details {
namespace SoundSystem {
namespace OpenAL {

class AudioBufferAL final: Noncopyable {
public:
	AudioBufferAL(void const* data, std::size_t size, AudioChannels channel,
		std::uint16_t bitsPerSample, std::uint32_t sampleRate);

	~AudioBufferAL();

	ALuint NativeBuffer() const;

private:
	Optional<ALuint> buffer;
};

}// namespace OpenAL
}// namespace SoundSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_AUDIOBUFFERAL_88AB8594_0292_40AB_84D9_F82EDDAD63A9_HPP)
