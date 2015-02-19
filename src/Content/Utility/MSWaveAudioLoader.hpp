//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_MSWAVEAUDIOLOADER_8808FBBE_6F7C_4FB1_84C9_CB1833D51283_HPP
#define POMDOG_MSWAVEAUDIOLOADER_8808FBBE_6F7C_4FB1_84C9_CB1833D51283_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Audio/AudioChannels.hpp"
#include "Pomdog/Application/Duration.hpp"
#include <cstdint>
#include <memory>
#include <string>

namespace Pomdog {

class AudioClip;

namespace Details {

struct MSWaveAudioLoader {
	static std::unique_ptr<AudioClip> Load(std::string const& filePath);
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_MSWAVEAUDIOLOADER_8808FBBE_6F7C_4FB1_84C9_CB1833D51283_HPP)
