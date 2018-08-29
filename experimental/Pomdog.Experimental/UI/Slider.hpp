// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/UI/UIElement.hpp"
#include "Pomdog.Experimental/UI/detail/UIEventConnection.hpp"
#include "Pomdog.Experimental/Rendering/Commands/PrimitivePolygonCommand.hpp"
#include "Pomdog.Experimental/Rendering/Commands/SpriteBatchCommand.hpp"
#include <Pomdog/Pomdog.hpp>
#include <optional>

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

class Slider final
    : public UIElement
    , public std::enable_shared_from_this<Slider> {
private:
    double minimum;
    double maximum;
    double value;

    struct ColorAnimation final {
        Color startColor;
        Color targetColor;
        float duration = 1;
        float time = 0;
    };

    std::optional<ColorAnimation> colorAnimation;
    Rendering::PrimitiveFunctionCommand renderCommand;
    Rendering::SpriteFontCommand spriteCommand;

    Detail::UIEventConnection connection;

    SliderColorScheme colorScheme;

    Color borderColor;
    Color fillColor;
    Color trackColor;
    bool isDragging;
    bool isEnabled;
    bool isTextVisible;

public:
    Slider(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        double minimum,
        double maximum);

    Slider(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        const SliderColorScheme& colorScheme,
        double minimum,
        double maximum);

    void SetValue(double valueIn);
    double GetValue() const;

    double GetMinimum() const;
    double GetMaximum() const;

    bool IsEnabled() const;
    void SetEnabled(bool isEnabled);

    void SetTextVisible(bool isTextVisible);

    VerticalAlignment GetVerticalAlignment() const noexcept override;

    bool SizeToFitContent() const override;

    void OnEnter() override;

    void OnPointerEntered(const PointerPoint& pointerPoint) override;

    void OnPointerExited(const PointerPoint& pointerPoint) override;

    void OnPointerPressed(const PointerPoint& pointerPoint) override;

    void OnPointerMoved(const PointerPoint& pointerPoint) override;

    void OnPointerReleased(const PointerPoint& pointerPoint) override;

    void Draw(DrawingContext & drawingContext) override;

    void UpdateAnimation(const Duration& frameDuration) override;

    Signal<void(float)> ValueChanged;
};

} // namespace UI
} // namespace Pomdog
