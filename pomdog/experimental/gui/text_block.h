// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/font_size.h"
#include "pomdog/experimental/gui/font_weight.h"
#include "pomdog/experimental/gui/text_alignment.h"
#include "pomdog/experimental/gui/widget.h"
#include "pomdog/math/color.h"
#include "pomdog/signals/scoped_connection.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {

class TextBlock final
    : public Widget
    , public std::enable_shared_from_this<TextBlock> {
public:
    explicit TextBlock(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    void
    setColor(const Color& color);

    void
    setFontWeight(FontWeight fontWeight);

    void
    setFontSize(FontSize fontSize);

    [[nodiscard]] std::string
    getText() const;

    void
    setText(const std::string& text);

    void
    setTextAlignment(TextAlignment textAlign);

    void
    setBaselineHeight(float pixelSize);

    void
    setHorizontalAlignment(HorizontalAlignment horizontalAlignment) noexcept;

    [[nodiscard]] HorizontalAlignment
    getHorizontalAlignment() const noexcept override;

    [[nodiscard]] VerticalAlignment
    getVerticalAlignment() const noexcept override;

    void
    draw(DrawingContext& drawingContext) override;

private:
    std::string text_;
    Color textColor_;
    float baselineHeight_;
    FontWeight fontWeight_;
    FontSize fontSize_;
    TextAlignment textAlignment_;
    HorizontalAlignment horizontalAlignment_;
};

} // namespace pomdog::gui
