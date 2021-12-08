// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class AudioClip;
class AudioEmitter;
class AudioEngine;
class AudioListener;
class SoundEffect;

enum class AudioChannels : std::uint8_t;
enum class SoundState : std::uint8_t;

} // namespace pomdog
