// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/widget.h"
#include "pomdog/math/color.h"
#include "pomdog/signals/scoped_connection.h"
#include "pomdog/signals/signal.h"

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

    [[nodiscard]] double
    getValue() const noexcept;

    void
    setValue(double value);

    [[nodiscard]] double
    getMinimum() const noexcept;

    void
    setMinimum(double minimum);

    [[nodiscard]] double
    getMaximum() const noexcept;

    void
    setMaximum(double maximum);

    [[nodiscard]] bool
    isEnabled() const noexcept;

    void
    setEnabled(bool enabled) noexcept;

    void
    setTextVisible(bool isTextVisible);

    void
    setThumbVisible(bool visible);

    [[nodiscard]] VerticalAlignment
    getVerticalAlignment() const noexcept override;

    [[nodiscard]] bool
    getSizeToFitContent() const noexcept override;

    void
    onEnter() override;

    void
    onPointerEntered(const PointerPoint& pointerPoint) override;

    void
    onPointerExited(const PointerPoint& pointerPoint) override;

    void
    onPointerPressed(const PointerPoint& pointerPoint) override;

    void
    onPointerMoved(const PointerPoint& pointerPoint) override;

    void
    onPointerReleased(const PointerPoint& pointerPoint) override;

    void
    draw(DrawingContext& drawingContext) override;

    void
    updateAnimation(const Duration& frameDuration) override;

    Signal<void(double)> ValueChanged;

private:
    struct ColorAnimation final {
        float duration = 1.0f;
        float time = 0.0f;
    };

    std::optional<ColorAnimation> colorAnimation_;
    ScopedConnection connection_;
    double minimum_;
    double maximum_;
    double value_;
    bool isDragging_;
    bool isEnabled_;
    bool isTextVisible_;
    bool isThumbVisible_;
    bool isPointerEntered_;
};

} // namespace pomdog::gui
