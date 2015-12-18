// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_MSWAVEAUDIOLOADER_8808FBBE_HPP
#define POMDOG_MSWAVEAUDIOLOADER_8808FBBE_HPP

#include "Pomdog/Audio/detail/ForwardDeclarations.hpp"
#include "Pomdog/Content/Utility/BinaryFileStream.hpp"
#include <memory>

namespace Pomdog {
namespace Detail {

struct MSWaveAudioLoader {
    static std::unique_ptr<AudioClip> Load(BinaryFileStream && binaryFile);
};

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_MSWAVEAUDIOLOADER_8808FBBE_HPP
