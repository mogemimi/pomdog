// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/GUI/HorizontalLine.hpp"
#include "Pomdog/Experimental/GUI/DrawingContext.hpp"
#include "Pomdog/Experimental/GUI/PointerPoint.hpp"
#include "Pomdog/Experimental/GUI/UIEventDispatcher.hpp"
#include "Pomdog/Experimental/GUI/UIHelper.hpp"
#include "Pomdog/Math/MathHelper.hpp"

namespace Pomdog::GUI {

HorizontalLine::HorizontalLine(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
{
    SetSize(10, 1);
}

HorizontalAlignment HorizontalLine::GetHorizontalAlignment() const noexcept
{
    return HorizontalAlignment::Stretch;
}

VerticalAlignment HorizontalLine::GetVerticalAlignment() const noexcept
{
    return VerticalAlignment::Top;
}

void HorizontalLine::Draw(DrawingContext& drawingContext)
{
    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());
    auto primitiveBatch = drawingContext.GetPrimitiveBatch();

    auto offset = MathHelper::ToVector2(globalPos);
    auto start = offset + Vector2{0.0f, 0.0f};
    auto end = offset + Vector2{static_cast<float>(GetWidth()), 0.0f};
    primitiveBatch->DrawLine(start, end, Color{92, 91, 90, 255}, 1.0f);
}

} // namespace Pomdog::GUI
