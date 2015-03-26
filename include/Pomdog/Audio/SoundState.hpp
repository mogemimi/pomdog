// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SOUNDSTATE_C171B07A_0FF0_4ABE_A2EB_461A2AD5C35B_HPP
#define POMDOG_SOUNDSTATE_C171B07A_0FF0_4ABE_A2EB_461A2AD5C35B_HPP

#include <cstdint>

namespace Pomdog {

enum class SoundState: std::uint8_t {
    Paused,

    Playing,

    Stopped,
};

}// namespace Pomdog

#endif // !defined(POMDOG_SOUNDSTATE_C171B07A_0FF0_4ABE_A2EB_461A2AD5C35B_HPP)
