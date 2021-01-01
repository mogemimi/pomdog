// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/GUI/Widget.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Signals/Signal.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::GUI {

class CheckBox final
    : public Widget
    , public std::enable_shared_from_this<CheckBox> {
public:
    explicit CheckBox(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    bool IsOn() const;
    void SetOn(bool isOn);

    bool IsEnabled() const;
    void SetEnabled(bool isEnabled);

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;
    VerticalAlignment GetVerticalAlignment() const noexcept override;

    void OnEnter() override;

    void OnPointerPressed(const PointerPoint& pointerPoint) override;

    void OnPointerReleased(const PointerPoint& pointerPoint) override;

    void Draw(DrawingContext& drawingContext) override;

    Signal<void(bool isOn)> Toggled;

private:
    ScopedConnection connection;
    bool isOn;
    bool isEnabled;
};

} // namespace Pomdog::GUI
