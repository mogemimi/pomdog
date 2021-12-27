// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/forward_declarations.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/chrono/duration.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::AudioHelper {

[[nodiscard]] std::size_t
GetSamples(std::size_t sizeInBytes, int bitsPerSample, AudioChannels channels) noexcept;

[[nodiscard]] Duration
GetSampleDuration(std::size_t samples, int sampleRate) noexcept;

} // namespace pomdog::detail::AudioHelper
