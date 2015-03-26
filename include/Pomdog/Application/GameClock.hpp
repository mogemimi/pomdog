// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GAMECLOCK_06D5BC45_786E_4B42_BE61_413BE23C6AF1_HPP
#define POMDOG_GAMECLOCK_06D5BC45_786E_4B42_BE61_413BE23C6AF1_HPP

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Event/Signal.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT GameClock {
public:
    GameClock();
    GameClock(GameClock const&) = delete;
    GameClock(GameClock &&) = default;

    GameClock & operator=(GameClock const&) = delete;
    GameClock & operator=(GameClock &&) = default;

    ~GameClock();

    void Tick();

    Duration TotalGameTime() const;

    std::uint32_t FrameNumber() const;

    Duration FrameDuration() const;

    float FrameRate() const;

    Duration ElapsedTime() const;

    Signal<void(Duration const& frameDuration)> OnTick;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

}// namespace Pomdog

#endif // !defined(POMDOG_GAMECLOCK_06D5BC45_786E_4B42_BE61_413BE23C6AF1_HPP)
