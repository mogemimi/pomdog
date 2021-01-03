// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/GUI/Widget.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Signals/Signal.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::GUI {

class VerticalScrollBar final
    : public Widget
    , public std::enable_shared_from_this<VerticalScrollBar> {
public:
    VerticalScrollBar(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        double minimum,
        double maximum);

    double GetValue() const noexcept;
    void SetValue(double value);

    double GetMinimum() const noexcept;
    void SetMinimum(double minimum) noexcept;

    double GetMaximum() const noexcept;
    void SetMaximum(double maximum) noexcept;

    double GetPageStep() const noexcept;
    void SetPageStep(double pageStep) noexcept;

    bool IsEnabled() const;
    void SetEnabled(bool isEnabled);

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;

    bool GetSizeToFitContent() const noexcept override;

    void OnEnter() override;

    void OnPointerEntered(const PointerPoint& pointerPoint) override;

    void OnPointerExited(const PointerPoint& pointerPoint) override;

    void OnPointerPressed(const PointerPoint& pointerPoint) override;

    void OnPointerMoved(const PointerPoint& pointerPoint) override;

    void OnPointerReleased(const PointerPoint& pointerPoint) override;

    void Draw(DrawingContext& drawingContext) override;

    void UpdateAnimation(const Duration& frameDuration) override;

    Signal<void(double)> ValueChanged;

private:
    struct ColorAnimation final {
        float duration = 1.0f;
        float time = 0.0f;
    };

    std::optional<ColorAnimation> colorAnimation;
    ScopedConnection connection;
    double minimum;
    double maximum;
    double value;
    double pageStep;
    std::optional<int> sliderGrabPosition;
    bool isEnabled;
    bool isPointerEntered;
};

} // namespace Pomdog::GUI
