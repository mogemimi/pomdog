// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/GUI/Widget.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Signals/Signal.hpp"
#include <memory>
#include <optional>

namespace Pomdog::GUI {

class Slider final
    : public Widget
    , public std::enable_shared_from_this<Slider> {
public:
    Slider(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        double minimum,
        double maximum);

    void SetValue(double valueIn);
    double GetValue() const;

    double GetMinimum() const;
    double GetMaximum() const;

    bool IsEnabled() const;
    void SetEnabled(bool isEnabled);

    void SetTextVisible(bool isTextVisible);

    void SetThumbVisible(bool visible);

    VerticalAlignment GetVerticalAlignment() const noexcept override;

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
    bool isDragging;
    bool isEnabled;
    bool isTextVisible;
    bool isThumbVisible;
    bool isPointerEntered;
};

} // namespace Pomdog::GUI
