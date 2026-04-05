// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/font_size.h"
#include "pomdog/experimental/gui/font_weight.h"
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

namespace pomdog::gui {
class PushButton;
class TextEdit;
class TextBlock;
} // namespace pomdog::gui

namespace pomdog::gui {

class NumberFieldDataContext {
public:
    virtual ~NumberFieldDataContext();

    [[nodiscard]] virtual std::string
    toString() const = 0;

    [[nodiscard]] virtual std::string
    toEditableString(const std::string& text) const = 0;

    virtual void
    incrementValue() = 0;

    virtual void
    decrementValue() = 0;

    virtual void
    beginDragging() = 0;

    virtual void
    updateDragging(int amount) = 0;

    virtual bool
    textSubmitted(const std::string& text) = 0;
};

class NumberField final
    : public Widget
    , public std::enable_shared_from_this<NumberField> {
public:
    NumberField(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        const std::shared_ptr<NumberFieldDataContext>& dataContext);

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

    void
    setText(const std::string& text);

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

    [[nodiscard]] std::shared_ptr<Widget>
    getChildAt(const Point2D& position) override;

    void
    updateAnimation(const Duration& frameDuration) override;

    void
    doLayout() override;

    void
    draw(DrawingContext& drawingContext) override;

    Signal<void()> ValueChanged;

private:
    std::shared_ptr<NumberFieldDataContext> dataContext_;
    std::shared_ptr<TextEdit> textEdit_;
    std::shared_ptr<TextBlock> textBlock_;
    std::shared_ptr<PushButton> addButton_;
    std::shared_ptr<PushButton> subButton_;

    ScopedConnection eventDispatchConn_;
    ScopedConnection textSubmittedConn_;
    ScopedConnection focusOutConn_;
    ScopedConnection addButtonConn_;
    ScopedConnection subButtonConn_;

    std::string prependAreaText_;
    Color prependAreaColor_;
    std::optional<Point2D> startDragPosition_;
    bool isPointerEntered_;
    bool isDragging_;
};

} // namespace pomdog::gui
