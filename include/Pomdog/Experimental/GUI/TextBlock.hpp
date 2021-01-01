// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/GUI/FontSize.hpp"
#include "Pomdog/Experimental/GUI/FontWeight.hpp"
#include "Pomdog/Experimental/GUI/TextAlignment.hpp"
#include "Pomdog/Experimental/GUI/Widget.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::GUI {

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

} // namespace Pomdog::GUI
