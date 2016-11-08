// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/UI/UIElement.hpp"
#include "Pomdog.Experimental/UI/FontSize.hpp"
#include "Pomdog.Experimental/UI/FontWeight.hpp"
#include "Pomdog.Experimental/UI/detail/UIEventConnection.hpp"
#include "Pomdog.Experimental/Rendering/Commands/SpriteBatchCommand.hpp"

namespace Pomdog {
namespace UI {

class TextBlock final
    : public UIElement
    , public std::enable_shared_from_this<TextBlock> {
public:
    explicit TextBlock(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    void SetColor(const Color& color);

    void SetFontWeight(FontWeight fontWeight);

    void SetFontSize(FontSize fontSize);

    std::string GetText() const;
    void SetText(const std::string& text);

    void SetHorizontalAlignment(HorizontalAlignment horizontalAlignment) noexcept;

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;
    VerticalAlignment GetVerticalAlignment() const noexcept override;

    void Draw(DrawingContext & drawingContext) override;

private:
    Rendering::SpriteFontCommand spriteCommand;
    std::string text;
    FontWeight fontWeight;
    FontSize fontSize;
    HorizontalAlignment horizontalAlignment;
};

} // namespace UI
} // namespace Pomdog
