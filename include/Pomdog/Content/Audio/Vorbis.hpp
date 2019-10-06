// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Audio/detail/ForwardDeclarations.hpp"
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
Load(const std::string& filename);

} // namespace Pomdog::Vorbis
