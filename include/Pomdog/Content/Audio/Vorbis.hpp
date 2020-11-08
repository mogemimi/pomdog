// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Audio/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <cstddef>
#include <memory>
#include <tuple>
#include <vector>

namespace Pomdog::Vorbis {

/// Reads a Ogg/Vorbis (.ogg) audio data from file stream.
[[nodiscard]] POMDOG_EXPORT
std::tuple<std::shared_ptr<AudioClip>, std::shared_ptr<Error>>
Load(const std::shared_ptr<AudioEngine>& audioEngine, const std::string& filename) noexcept;

} // namespace Pomdog::Vorbis
