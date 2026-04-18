// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/input/touchscreen.h"
#include "pomdog/math/point2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
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
    std::array<Point2D, 5> position_ = {};
    std::array<bool, 5> pressed_ = {};
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

    void handleMessage(const SystemEvent& event);

    void simulateUsingMouse(const Mouse& mouse) noexcept;
};

} // namespace pomdog::detail::emscripten
