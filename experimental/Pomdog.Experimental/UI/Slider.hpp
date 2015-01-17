// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "UIElement.hpp"
#include "detail/UIEventConnection.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {
namespace UI {

struct SliderColorScheme {
    Color BorderColor {0, 140, 198, 255};
    Color FillColor1 {27, 161, 226, 255};
    Color FillColor2 {56, 190, 255, 255};
    Color DisabledFillColor {150, 161, 167, 255};
    Color TrackColor {198, 198, 198, 255};
    Color ThumbColor = Color::Black;
    Color FocusedThumbColor {229, 20, 0, 255};
};

class Slider: public UIElement, public std::enable_shared_from_this<Slider> {
private:
    double minimum;
    double maximum;
    double value;

    struct ColorAnimation {
        Color startColor;
        Color targetColor;
        float duration = 1;
        float time = 0;
    };

    Optional<ColorAnimation> colorAnimation;

    Detail::UIEventConnection connection;

    SliderColorScheme colorScheme;

    Color borderColor;
    Color fillColor;
    Color trackColor;
    bool isDragging;
    bool isEnabled;

public:
    Slider(
        std::shared_ptr<UIEventDispatcher> const& dispatcher,
        double minimum,
        double maximum);

    Slider(
        std::shared_ptr<UIEventDispatcher> const& dispatcher,
        SliderColorScheme const& colorScheme,
        double minimum,
        double maximum);

    void Value(double valueIn);
    double Value() const;

    double Minimum() const;
    double Maximum() const;

    bool IsEnabled() const;
    void IsEnabled(bool isEnabled);

    UI::VerticalAlignment VerticalAlignment() const override { return UI::VerticalAlignment::Top; }

    bool SizeToFitContent() const override { return false; }

    void OnEnter() override;

    void OnPointerEntered(PointerPoint const& pointerPoint) override;

    void OnPointerExited(PointerPoint const& pointerPoint) override;

    void OnPointerPressed(PointerPoint const& pointerPoint) override;

    void OnPointerMoved(PointerPoint const& pointerPoint) override;

    void OnPointerReleased(PointerPoint const& pointerPoint) override;

    void OnRenderSizeChanged(int width, int height) override;

    void Draw(DrawingContext & drawingContext) override;

    void UpdateAnimation(Duration const& frameDuration) override;

    Signal<void(float)> ValueChanged;
};

} // namespace UI
} // namespace Pomdog
