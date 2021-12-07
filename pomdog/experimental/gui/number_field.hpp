// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/gui/font_size.hpp"
#include "pomdog/experimental/gui/font_weight.hpp"
#include "pomdog/experimental/gui/thickness.hpp"
#include "pomdog/experimental/gui/widget.hpp"
#include "pomdog/math/color.hpp"
#include "pomdog/signals/scoped_connection.hpp"
#include "pomdog/signals/signal.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::GUI {

class PushButton;
class TextEdit;
class TextBlock;

class NumberFieldDataContext {
public:
    virtual ~NumberFieldDataContext();

    virtual std::string ToString() const = 0;

    virtual std::string ToEditableString(const std::string& text) const = 0;

    virtual void IncrementValue() = 0;

    virtual void DecrementValue() = 0;

    virtual void BeginDragging() = 0;

    virtual void UpdateDragging(int amount) = 0;

    virtual bool TextSubmitted(const std::string& text) = 0;
};

class NumberField final
    : public Widget
    , public std::enable_shared_from_this<NumberField> {
public:
    NumberField(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        const std::shared_ptr<NumberFieldDataContext>& dataContext);

    void SetFontWeight(FontWeight fontWeight);

    void SetFontSize(FontSize fontSize);

    bool IsEnabled() const;
    void SetEnabled(bool isEnabled);

    bool IsReadOnly() const;
    void SetReadOnly(bool readOnly);

    bool IsAcceptable() const;
    void SetAcceptable(bool acceptable);

    void SetText(const std::string& text);

    std::string GetPlaceholderText() const;
    void SetPlaceholderText(const std::string& text);

    bool HasSelectedText() const;

    void Deselect();

    void SetPrependAreaColor(const std::optional<Color>& color);

    void SetPrependAreaString(const std::string& text);

    void SetHorizontalAlignment(HorizontalAlignment horizontalAlignment) noexcept;

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;
    VerticalAlignment GetVerticalAlignment() const noexcept override;

    void SetPosition(const Point2D& position) override;

    void MarkParentTransformDirty() override;

    void OnEnter() override;

    void OnPointerEntered(const PointerPoint& pointerPoint) override;

    void OnPointerExited(const PointerPoint& pointerPoint) override;

    void OnPointerPressed(const PointerPoint& pointerPoint) override;

    void OnPointerMoved(const PointerPoint& pointerPoint) override;

    void OnPointerReleased(const PointerPoint& pointerPoint) override;

    std::shared_ptr<Widget> GetChildAt(const Point2D& position) override;

    void UpdateAnimation(const Duration& frameDuration) override;

    void DoLayout() override;

    void Draw(DrawingContext& drawingContext) override;

    Signal<void()> ValueChanged;

private:
    std::shared_ptr<NumberFieldDataContext> dataContext;
    std::shared_ptr<TextEdit> textEdit;
    std::shared_ptr<TextBlock> textBlock;
    std::shared_ptr<PushButton> addButton;
    std::shared_ptr<PushButton> subButton;

    ScopedConnection eventDispatchConn;
    ScopedConnection textSubmittedConn;
    ScopedConnection focusOutConn;
    ScopedConnection addButtonConn;
    ScopedConnection subButtonConn;

    std::string prependAreaText;
    Color prependAreaColor;
    std::optional<Point2D> startDragPosition;
    bool isPointerEntered;
    bool isDragging;
};

} // namespace Pomdog::GUI
