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
class TextEdit;
} // namespace pomdog::gui

namespace pomdog::gui {

class TextField final
    : public Widget
    , public std::enable_shared_from_this<TextField> {
public:
    explicit TextField(const std::shared_ptr<UIEventDispatcher>& dispatcher);

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

    [[nodiscard]] std::string
    getText() const;

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
    setHorizontalAlignment(HorizontalAlignment horizontalAlignment) noexcept;

    [[nodiscard]] HorizontalAlignment
    getHorizontalAlignment() const noexcept override;

    [[nodiscard]] VerticalAlignment
    getVerticalAlignment() const noexcept override;

    void
    setPosition(const Point2D& position) override;

    void
    markParentTransformDirty() override;

    [[nodiscard]] std::shared_ptr<Widget>
    getChildAt(const Point2D& position) override;

    void
    updateAnimation(const Duration& frameDuration) override;

    void
    onEnter() override;

    void
    doLayout() override;

    void
    draw(DrawingContext& drawingContext) override;

    Signal<void()> TextChanged;

private:
    std::shared_ptr<TextEdit> textEdit_;
    ScopedConnection eventDispatchConn_;
    ScopedConnection textChangedConn_;
};

} // namespace pomdog::gui
