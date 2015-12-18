// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_GAMECLOCK_06D5BC45_HPP
#define POMDOG_GAMECLOCK_06D5BC45_HPP

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Signals/Signal.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT GameClock {
public:
    GameClock();

    explicit GameClock(int framesPerSecond);

    GameClock(GameClock const&) = delete;
    GameClock(GameClock &&) = default;

    GameClock & operator=(GameClock const&) = delete;
    GameClock & operator=(GameClock &&) = default;

    ~GameClock();

    void Tick();

    Duration TotalGameTime() const noexcept;

    std::uint32_t FrameNumber() const noexcept;

    Duration FrameDuration() const noexcept;

    float FrameRate() const noexcept;

    Duration ElapsedTime() const noexcept;

    Signal<void(Duration const& frameDuration)> OnTick;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog

#endif // POMDOG_GAMECLOCK_06D5BC45_HPP
