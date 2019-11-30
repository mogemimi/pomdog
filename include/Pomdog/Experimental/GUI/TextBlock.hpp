// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/GUI/FontSize.hpp"
#include "Pomdog/Experimental/GUI/FontWeight.hpp"
#include "Pomdog/Experimental/GUI/Widget.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include <memory>
#include <string>

namespace Pomdog::GUI {

enum class TextAlignment : std::uint8_t {
    Center,
    Left,
    Right,
};

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
