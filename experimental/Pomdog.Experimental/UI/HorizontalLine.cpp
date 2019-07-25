// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/UI/HorizontalLine.hpp"
#include "Pomdog.Experimental/UI/DrawingContext.hpp"
#include "Pomdog.Experimental/UI/PointerPoint.hpp"
#include "Pomdog.Experimental/UI/UIHelper.hpp"
#include "Pomdog.Experimental/UI/UIEventDispatcher.hpp"

namespace Pomdog {
namespace UI {

HorizontalLine::HorizontalLine(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : UIElement(dispatcher)
{
    SetSize(10, 1);
}

HorizontalAlignment HorizontalLine::GetHorizontalAlignment() const noexcept
{
    return UI::HorizontalAlignment::Stretch;
}

VerticalAlignment HorizontalLine::GetVerticalAlignment() const noexcept
{
    return UI::VerticalAlignment::Top;
}

void HorizontalLine::Draw(DrawingContext & drawingContext)
{
    auto transform = GetTransform() * drawingContext.Top();

    renderCommand.SetInvoker([this, transform](PrimitiveBatch& primitiveBatch) {
        auto offset = Vector2{transform(2, 0), transform(2, 1)};
        auto start = offset + Vector2{0.0f, 0.0f};
        auto end = offset + Vector2{static_cast<float>(GetWidth()), 0.0f};
        primitiveBatch.DrawLine(start, end, Color{92, 91, 90, 255}, 1.0f);
    });
    drawingContext.PushCommand(renderCommand);
}

} // namespace UI
} // namespace Pomdog
