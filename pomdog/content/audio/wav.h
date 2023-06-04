// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/forward_declarations.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <fstream>
#include <memory>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::WAV {

/// Reads an audio data from Waveform Audio File (.wav) format data.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
load(const std::shared_ptr<AudioEngine>& audioEngine, std::ifstream&& stream, std::size_t byteLength) noexcept;

} // namespace pomdog::WAV
