// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/GUI/FontWeight.hpp"
#include "Pomdog/Experimental/GUI/TextAlignment.hpp"
#include "Pomdog/Experimental/GUI/Widget.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Signals/Signal.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::GUI {

class PushButton final
    : public Widget
    , public std::enable_shared_from_this<PushButton> {
public:
    explicit PushButton(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    bool IsEnabled() const;
    void SetEnabled(bool isEnabled);

    bool IsHovered() const;
    bool IsFocused() const;

    void SetFontWeight(FontWeight fontWeight);

    std::string GetText() const;
    void SetText(const std::string& text);

    void SetTextAlignment(TextAlignment textAlign);

    void SetHorizontalAlignment(HorizontalAlignment horizontalAlignment) noexcept;

    void SetVerticalAlignment(VerticalAlignment verticalAlignment) noexcept;

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;
    VerticalAlignment GetVerticalAlignment() const noexcept override;

    void OnEnter() override;

    void OnFocusIn() override;

    void OnFocusOut() override;

    void OnPointerEntered(const PointerPoint& pointerPoint) override;

    void OnPointerExited(const PointerPoint& pointerPoint) override;

    void OnPointerPressed(const PointerPoint& pointerPoint) override;

    void OnPointerReleased(const PointerPoint& pointerPoint) override;

    void Draw(DrawingContext& drawingContext) override;

    Signal<void()> Click;

private:
    ScopedConnection connection;
    std::string text;
    FontWeight fontWeight;
    TextAlignment textAlignment;
    HorizontalAlignment horizontalAlignment;
    VerticalAlignment verticalAlignment;
    bool isEnabled;
    bool isHovered;
    bool isPressed;
    bool isFocused;
};

} // namespace Pomdog::GUI
