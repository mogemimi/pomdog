// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "UIElement.hpp"
#include "detail/UIEventConnection.hpp"

namespace Pomdog {
namespace UI {

class TextBlock: public UIElement, public std::enable_shared_from_this<TextBlock> {
public:
    explicit TextBlock(std::shared_ptr<UIEventDispatcher> const& dispatcher);

    std::string Text() const;
    void Text(std::string const& text);

    UI::HorizontalAlignment HorizontalAlignment() const override { return UI::HorizontalAlignment::Stretch; }
    UI::VerticalAlignment VerticalAlignment() const override { return UI::VerticalAlignment::Top; }

    void OnRenderSizeChanged(int width, int height) override;

    void Draw(DrawingContext & drawingContext) override;

private:
    std::string text;
};

} // namespace UI
} // namespace Pomdog
