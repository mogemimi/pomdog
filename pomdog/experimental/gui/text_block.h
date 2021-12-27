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

    void SetColor(const Color& color);

    void SetFontWeight(FontWeight fontWeight);

    void SetFontSize(FontSize fontSize);

    std::string GetText() const;
    void SetText(const std::string& text);

    void SetTextAlignment(TextAlignment textAlign);

    void SetBaselineHeight(float pixelSize);

    void SetHorizontalAlignment(HorizontalAlignment horizontalAlignment) noexcept;

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;
    VerticalAlignment GetVerticalAlignment() const noexcept override;

    void Draw(DrawingContext& drawingContext) override;

private:
    std::string text;
    Color textColor;
    float baselineHeight;
    FontWeight fontWeight;
    FontSize fontSize;
    TextAlignment textAlignment;
    HorizontalAlignment horizontalAlignment;
};

} // namespace pomdog::gui
