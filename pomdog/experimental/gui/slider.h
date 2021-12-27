// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/gui/widget.hpp"
#include "pomdog/math/color.hpp"
#include "pomdog/signals/scoped_connection.hpp"
#include "pomdog/signals/signal.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {

class Slider final
    : public Widget
    , public std::enable_shared_from_this<Slider> {
public:
    Slider(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        double minimum,
        double maximum);

    double GetValue() const noexcept;
    void SetValue(double value);

    double GetMinimum() const noexcept;
    void SetMinimum(double minimum);

    double GetMaximum() const noexcept;
    void SetMaximum(double maximum);

    bool IsEnabled() const noexcept;
    void SetEnabled(bool enabled) noexcept;

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

} // namespace pomdog::gui
