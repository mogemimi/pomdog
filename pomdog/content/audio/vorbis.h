// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/forward_declarations.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <memory>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::Vorbis {

/// Reads a Ogg/Vorbis (.ogg) audio data from file stream.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
Load(const std::shared_ptr<AudioEngine>& audioEngine, const std::string& filename) noexcept;

} // namespace pomdog::Vorbis
