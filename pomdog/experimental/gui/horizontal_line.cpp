// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/horizontal_line.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/math/math.h"

namespace pomdog::gui {

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

    auto offset = math::ToVector2(globalPos);
    auto start = offset + Vector2{0.0f, 0.0f};
    auto end = offset + Vector2{static_cast<float>(GetWidth()), 0.0f};
    primitiveBatch->DrawLine(start, end, Color{92, 91, 90, 255}, 1.0f);
}

} // namespace pomdog::gui
