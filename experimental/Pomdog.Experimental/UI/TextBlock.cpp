// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "TextBlock.hpp"
#include "DrawingContext.hpp"
#include "PointerPoint.hpp"
#include "UIHelper.hpp"
#include "UIEventDispatcher.hpp"

namespace Pomdog {
namespace UI {
//-----------------------------------------------------------------------
TextBlock::TextBlock()
	: UIView(Matrix3x2::Identity, 50, 15)
	, text()
{}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Properties
#endif
//-----------------------------------------------------------------------
std::string TextBlock::Text() const
{ return this->text; }
//-----------------------------------------------------------------------
void TextBlock::Text(std::string const& textIn)
{ this->text = textIn; }
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Events
#endif
//-----------------------------------------------------------------------
void TextBlock::OnRenderSizeChanged(std::uint32_t widthIn, std::uint32_t heightIn)
{
	Width(widthIn);
	Height(heightIn);
}
//-----------------------------------------------------------------------
void TextBlock::Draw(DrawingContext & drawingContext)
{
	auto transform = Transform() * drawingContext.Top();

	drawingContext.DrawString(transform, Color::White, FontWeight::Normal, FontSize::Medium, text);
}
//-----------------------------------------------------------------------
}// namespace UI
}// namespace Pomdog
