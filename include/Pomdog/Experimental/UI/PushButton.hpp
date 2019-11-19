// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/UI/UIElement.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Signals/Signal.hpp"
#include <memory>

namespace Pomdog::UI {

class PushButton final
    : public UIElement
    , public std::enable_shared_from_this<PushButton> {
public:
    explicit PushButton(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    bool IsEnabled() const;
    void SetEnabled(bool isEnabled);

    void SetText(const std::string& text);

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;
    VerticalAlignment GetVerticalAlignment() const noexcept override;

    void OnEnter() override;

    void OnPointerEntered(const PointerPoint& pointerPoint) override;

    void OnPointerExited(const PointerPoint& pointerPoint) override;

    void OnPointerPressed(const PointerPoint& pointerPoint) override;

    void OnPointerReleased(const PointerPoint& pointerPoint) override;

    void Draw(DrawingContext& drawingContext) override;

    // Events:
    Signal<void()> Click;

private:
    ScopedConnection connection;
    std::string text;
    bool isEnabled;
    bool isHovered;
    bool isPressed;
};

} // namespace Pomdog::UI
