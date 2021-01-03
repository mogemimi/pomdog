// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Audio/ForwardDeclarations.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <memory>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Vorbis {

/// Reads a Ogg/Vorbis (.ogg) audio data from file stream.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
Load(const std::shared_ptr<AudioEngine>& audioEngine, const std::string& filename) noexcept;

} // namespace Pomdog::Vorbis
