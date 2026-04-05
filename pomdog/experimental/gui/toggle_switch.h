// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/gui/widget.h"
#include "pomdog/signals/scoped_connection.h"
#include "pomdog/signals/signal.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {

class ToggleSwitch final
    : public Widget
    , public std::enable_shared_from_this<ToggleSwitch> {
public:
    explicit ToggleSwitch(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    [[nodiscard]] bool
    isOn() const;

    void
    setOn(bool isOn);

    [[nodiscard]] bool
    isEnabled() const;

    void
    setEnabled(bool isEnabled);

    [[nodiscard]] HorizontalAlignment
    getHorizontalAlignment() const noexcept override;

    [[nodiscard]] VerticalAlignment
    getVerticalAlignment() const noexcept override;

    void
    onEnter() override;

    void
    onPointerPressed(const PointerPoint& pointerPoint) override;

    void
    onPointerReleased(const PointerPoint& pointerPoint) override;

    void
    updateAnimation(const Duration& frameDuration) override;

    void
    draw(DrawingContext& drawingContext) override;

    Signal<void(bool isOn)> Toggled;

private:
    enum class ToggleAnimation : i8 {
        Stopped,
        OnToOff,
        OffToOn,
    };

    ToggleAnimation toggleAnimation_;
    float animationDuration_;

    ScopedConnection connection_;
    bool isOn_;
    bool isEnabled_;
};

} // namespace pomdog::gui
