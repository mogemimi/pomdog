// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
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

    bool IsOn() const;
    void SetOn(bool isOn);

    bool IsEnabled() const;
    void SetEnabled(bool isEnabled);

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;
    VerticalAlignment GetVerticalAlignment() const noexcept override;

    void OnEnter() override;

    void OnPointerPressed(const PointerPoint& pointerPoint) override;

    void OnPointerReleased(const PointerPoint& pointerPoint) override;

    void UpdateAnimation(const Duration& frameDuration) override;

    void Draw(DrawingContext& drawingContext) override;

    // Events:
    Signal<void(bool isOn)> Toggled;

private:
    enum class ToggleAnimation : std::int8_t {
        Stopped,
        OnToOff,
        OffToOn,
    };

    ToggleAnimation toggleAnimation;
    float animationDuration;

    ScopedConnection connection;
    bool isOn;
    bool isEnabled;
};

} // namespace pomdog::gui
