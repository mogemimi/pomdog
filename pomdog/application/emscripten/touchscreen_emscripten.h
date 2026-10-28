// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/input/touchscreen.h"
#include "pomdog/math/point2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cstddef>
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
class SystemEvent;
class SystemEventQueue;
} // namespace pomdog::detail

namespace pomdog {
class Mouse;
} // namespace pomdog

namespace pomdog::detail::emscripten {

class TouchscreenEmscripten final : public Touchscreen {
private:
    static constexpr std::size_t maxTouchCount = 5;

    // NOTE: Each touch point holds the slot it got when it started until it
    // ends. The slot index is the touch index of the public API.
    std::array<Point2D, maxTouchCount> position_ = {};
    std::array<i32, maxTouchCount> identifiers_ = {};
    std::array<bool, maxTouchCount> pressed_ = {};

    // NOTE: A tap shorter than a frame ends in the frame it starts. It stays
    // pressed for that frame and is released when the next frame begins, so
    // that the game sees it.
    std::array<bool, maxTouchCount> startedInFrame_ = {};
    std::array<bool, maxTouchCount> releasePending_ = {};
    bool mouseSimulationEnabled_ = false;

public:
    TouchscreenEmscripten() noexcept;

    ~TouchscreenEmscripten() noexcept override;

    [[nodiscard]] bool
    isMouseSimulationEnabled() const noexcept override;

    void
    setMouseSimulationEnabled(bool enabled) noexcept override;

    [[nodiscard]] u8
    getSupportedTouchCount() const noexcept override;

    [[nodiscard]] Point2D
    getPosition(u8 touchIndex) const noexcept override;

    [[nodiscard]] bool
    isTouchPressed(u8 touchIndex) const noexcept override;

    [[nodiscard]] Point2D
    getPrimaryTouchPosition() const noexcept override;

    [[nodiscard]] bool
    isPrimaryTouchPressed() const noexcept override;

    void subscribeEvent(const std::string& targetCanvas, const std::shared_ptr<SystemEventQueue>& eventQueue) noexcept;

    void unsubscribeEvent(const std::string& targetCanvas) noexcept;

    /// Releases the touches that ended in the previous frame right after they
    /// started. Call it at the beginning of each frame, before the events.
    void beginFrame() noexcept;

    void handleMessage(const SystemEvent& event);

    void simulateUsingMouse(const Mouse& mouse) noexcept;
};

} // namespace pomdog::detail::emscripten
