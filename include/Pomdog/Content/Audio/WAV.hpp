// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Audio/ForwardDeclarations.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <fstream>
#include <memory>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::WAV {

/// Reads an audio data from Waveform Audio File (.wav) format data.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
Load(const std::shared_ptr<AudioEngine>& audioEngine, std::ifstream&& stream, std::size_t byteLength) noexcept;

} // namespace Pomdog::WAV
