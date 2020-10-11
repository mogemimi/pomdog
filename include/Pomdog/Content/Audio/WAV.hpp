// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Audio/detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <cstddef>
#include <fstream>
#include <memory>
#include <tuple>
#include <vector>

namespace Pomdog::WAV {

/// Reads an audio data from Waveform Audio File (.wav) format data.
[[nodiscard]] POMDOG_EXPORT
std::tuple<std::shared_ptr<AudioClip>, std::shared_ptr<Error>>
Load(const std::shared_ptr<AudioEngine>& audioEngine, std::ifstream&& stream, std::size_t byteLength) noexcept;

} // namespace Pomdog::WAV
