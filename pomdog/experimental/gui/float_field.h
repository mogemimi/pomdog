// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/font_size.h"
#include "pomdog/experimental/gui/font_weight.h"
#include "pomdog/experimental/gui/number_field.h"
#include "pomdog/experimental/gui/thickness.h"
#include "pomdog/experimental/gui/widget.h"
#include "pomdog/math/color.h"
#include "pomdog/signals/scoped_connection.h"
#include "pomdog/signals/signal.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui::detail {

class FloatFieldDataContext final : public NumberFieldDataContext {
public:
    [[nodiscard]] std::string
    toString() const override;

    [[nodiscard]] std::string
    toEditableString(const std::string& text) const override;

    [[nodiscard]] double
    getValue() const noexcept;

    void
    setValue(double valueIn);

    [[nodiscard]] double
    getMinimum() const noexcept;

    void
    setMinimum(double minimumIn);

    [[nodiscard]] double
    getMaximum() const noexcept;

    void
    setMaximum(double maximumIn);

    [[nodiscard]] int
    getDecimals() const noexcept;

    void
    setDecimals(int precisionIn);

    [[nodiscard]] double
    getUnit() const noexcept;

    void
    incrementValue() override;

    void
    decrementValue() override;

    void
    beginDragging() override;

    void
    updateDragging(int amount) override;

    bool
    textSubmitted(const std::string& text) override;

private:
    double value_ = 0.0;
    double startDragValue_ = 0.0;
    double minimum_ = std::numeric_limits<double>::lowest();
    double maximum_ = std::numeric_limits<double>::max();
    int precision_ = 3;
};

} // namespace pomdog::gui::detail

namespace pomdog::gui {

class FloatField final
    : public Widget
    , public std::enable_shared_from_this<FloatField> {
public:
    explicit FloatField(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    void
    setFontWeight(FontWeight fontWeight);

    void
    setFontSize(FontSize fontSize);

    [[nodiscard]] bool
    isEnabled() const;

    void
    setEnabled(bool isEnabled);

    [[nodiscard]] bool
    isReadOnly() const;

    void
    setReadOnly(bool readOnly);

    [[nodiscard]] bool
    isAcceptable() const;

    void
    setAcceptable(bool acceptable);

    [[nodiscard]] double
    getValue() const;

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

    [[nodiscard]] int
    getDecimals() const;

    void
    setDecimals(int precision);

    [[nodiscard]] std::string
    getPlaceholderText() const;

    void
    setPlaceholderText(const std::string& text);

    [[nodiscard]] bool
    hasSelectedText() const;

    void
    deselect();

    void
    setPrependAreaColor(const std::optional<Color>& color);

    void
    setPrependAreaString(const std::string& text);

    void
    setHorizontalAlignment(HorizontalAlignment horizontalAlignment) noexcept;

    [[nodiscard]] HorizontalAlignment
    getHorizontalAlignment() const noexcept override;

    [[nodiscard]] VerticalAlignment
    getVerticalAlignment() const noexcept override;

    void
    setPosition(const Point2D& position) override;

    void
    markParentTransformDirty() override;

    void
    onEnter() override;

    [[nodiscard]] std::shared_ptr<Widget>
    getChildAt(const Point2D& position) override;

    void
    updateAnimation(const Duration& frameDuration) override;

    void
    doLayout() override;

    void
    draw(DrawingContext& drawingContext) override;

    Signal<void(double)> ValueChanged;

private:
    std::shared_ptr<NumberField> numberField_;
    std::shared_ptr<detail::FloatFieldDataContext> dataContext_;
    ScopedConnection valueChangedConn_;
};

} // namespace pomdog::gui
