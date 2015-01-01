//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_TEXTBLOCK_0351BBF2_DD3C_42E0_BF05_E9A41E762279_HPP
#define POMDOG_TEXTBLOCK_0351BBF2_DD3C_42E0_BF05_E9A41E762279_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "UIView.hpp"
#include "detail/UIEventConnection.hpp"

namespace Pomdog {
namespace UI {

class TextBlock: public UIView, public std::enable_shared_from_this<TextBlock> {
public:
	TextBlock();

	std::string Text() const;
	void Text(std::string const& text);

	UI::HorizontalAlignment HorizontalAlignment() const override { return UI::HorizontalAlignment::Stretch; }
	UI::VerticalAlignment VerticalAlignment() const override { return UI::VerticalAlignment::Top; }

	void OnRenderSizeChanged(std::uint32_t width, std::uint32_t height) override;
	
	void Draw(DrawingContext & drawingContext) override;
	
private:
	std::string text;
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_TEXTBLOCK_0351BBF2_DD3C_42E0_BF05_E9A41E762279_HPP)
