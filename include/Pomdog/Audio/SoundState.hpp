// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class SoundState : std::uint8_t {
    Paused,
    Playing,
    Stopped,
};

} // namespace Pomdog
