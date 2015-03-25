// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

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

namespace Detail {

struct MSWaveAudioLoader {
    static std::unique_ptr<AudioClip> Load(std::string const& filePath);
};

}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_MSWAVEAUDIOLOADER_8808FBBE_6F7C_4FB1_84C9_CB1833D51283_HPP)
