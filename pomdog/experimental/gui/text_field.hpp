// Copyright mogemimi. Distributed under the MIT license.

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

class TextEdit;

class TextField final
    : public Widget
    , public std::enable_shared_from_this<TextField> {
public:
    explicit TextField(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    void SetFontWeight(FontWeight fontWeight);

    void SetFontSize(FontSize fontSize);

    bool IsEnabled() const;
    void SetEnabled(bool isEnabled);

    bool IsReadOnly() const;
    void SetReadOnly(bool readOnly);

    bool IsAcceptable() const;
    void SetAcceptable(bool acceptable);

    std::string GetText() const;
    void SetText(const std::string& text);

    std::string GetPlaceholderText() const;
    void SetPlaceholderText(const std::string& text);

    // void SetTextMargin(const Thickness& margin);
    // void SetMaximumLength(int maxLength);
    // std::string GetSelectedText() const;

    bool HasSelectedText() const;

    void Deselect();

    void SetHorizontalAlignment(HorizontalAlignment horizontalAlignment) noexcept;

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;
    VerticalAlignment GetVerticalAlignment() const noexcept override;

    void SetPosition(const Point2D& position) override;

    void MarkParentTransformDirty() override;

    std::shared_ptr<Widget> GetChildAt(const Point2D& position) override;

    void UpdateAnimation(const Duration& frameDuration) override;

    void OnEnter() override;

    void DoLayout() override;

    void Draw(DrawingContext& drawingContext) override;

    Signal<void()> TextChanged;

private:
    std::shared_ptr<TextEdit> textEdit;
    ScopedConnection eventDispatchConn;
    ScopedConnection textChangedConn;
};

} // namespace Pomdog::GUI
