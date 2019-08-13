// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Audio/detail/ForwardDeclarations.hpp"
#include "Pomdog/Content/Utility/BinaryFileStream.hpp"
#include <memory>

namespace Pomdog::Detail {

struct MSWaveAudioLoader {
    static std::unique_ptr<AudioClip> Load(BinaryFileStream && binaryFile);
};

} // namespace Pomdog::Detail
