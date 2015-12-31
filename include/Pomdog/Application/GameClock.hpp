// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

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

    Duration GetTotalGameTime() const noexcept;

    std::uint32_t GetFrameNumber() const noexcept;

    Duration GetFrameDuration() const noexcept;

    float GetFrameRate() const noexcept;

    Duration GetElapsedTime() const noexcept;

    Signal<void(Duration const& frameDuration)> OnTick;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog
