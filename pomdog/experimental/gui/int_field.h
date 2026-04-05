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

class IntFieldDataContext final : public NumberFieldDataContext {
public:
    [[nodiscard]] std::string
    toString() const override;

    [[nodiscard]] std::string
    toEditableString(const std::string& text) const override;

    [[nodiscard]] int
    getValue() const noexcept;

    void
    setValue(int valueIn);

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
    int value_ = 0;
    int startDragValue_ = 0;
};

} // namespace pomdog::gui::detail

namespace pomdog::gui {

class IntField final
    : public Widget
    , public std::enable_shared_from_this<IntField> {
public:
    explicit IntField(const std::shared_ptr<UIEventDispatcher>& dispatcher);

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

    [[nodiscard]] int
    getValue() const;

    void
    setValue(int value);

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

    Signal<void(int)> ValueChanged;

private:
    std::shared_ptr<NumberField> numberField_;
    std::shared_ptr<detail::IntFieldDataContext> dataContext_;
    ScopedConnection valueChangedConn_;
};

} // namespace pomdog::gui
