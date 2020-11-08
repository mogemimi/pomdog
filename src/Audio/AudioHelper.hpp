// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Audio/ForwardDeclarations.hpp"
#include <cstddef>

namespace Pomdog::Detail::AudioHelper {

[[nodiscard]] std::size_t
GetSamples(std::size_t sizeInBytes, int bitsPerSample, AudioChannels channels) noexcept;

[[nodiscard]] Duration
GetSampleDuration(std::size_t samples, int sampleRate) noexcept;

} // namespace Pomdog::Detail::AudioHelper
