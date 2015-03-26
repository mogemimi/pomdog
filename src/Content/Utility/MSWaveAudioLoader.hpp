// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_MSWAVEAUDIOLOADER_8808FBBE_HPP
#define POMDOG_MSWAVEAUDIOLOADER_8808FBBE_HPP

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

#endif // POMDOG_MSWAVEAUDIOLOADER_8808FBBE_HPP
