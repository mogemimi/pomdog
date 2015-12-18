// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_SLIDER_8DA97A75_HPP
#define POMDOG_SLIDER_8DA97A75_HPP

#include "Control.hpp"
#include "UIView.hpp"
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

class Slider: public Control, public std::enable_shared_from_this<Slider> {
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
    Slider(double minimum, double maximum);

    Slider(SliderColorScheme const& colorScheme, double minimum, double maximum);

    void Value(double valueIn);
    double Value() const;

    double Minimum() const;
    double Maximum() const;

    bool IsEnabled() const;
    void IsEnabled(bool isEnabled);

    UI::VerticalAlignment VerticalAlignment() const override { return UI::VerticalAlignment::Top; }

    bool SizeToFitContent() const override { return false; }

    void OnParentChanged() override;

    void OnPointerEntered(PointerPoint const& pointerPoint) override;

    void OnPointerExited(PointerPoint const& pointerPoint) override;

    void OnPointerPressed(PointerPoint const& pointerPoint) override;

    void OnPointerMoved(PointerPoint const& pointerPoint) override;

    void OnPointerReleased(PointerPoint const& pointerPoint) override;

    void OnRenderSizeChanged(std::uint32_t width, std::uint32_t height) override;

    void Draw(DrawingContext & drawingContext) override;

    void UpdateAnimation(Duration const& frameDuration) override;
};

}// namespace UI
}// namespace Pomdog

#endif // POMDOG_SLIDER_8DA97A75_HPP
