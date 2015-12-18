// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_SOUNDSTATE_C171B07A_HPP
#define POMDOG_SOUNDSTATE_C171B07A_HPP

#include <cstdint>

namespace Pomdog {

enum class SoundState: std::uint8_t {
    Paused,
    Playing,
    Stopped,
};

} // namespace Pomdog

#endif // POMDOG_SOUNDSTATE_C171B07A_HPP
