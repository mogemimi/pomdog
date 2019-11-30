// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/GUI/Widget.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Signals/Signal.hpp"
#include <memory>

namespace Pomdog::GUI {

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

} // namespace Pomdog::GUI
