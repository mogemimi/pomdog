// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/context_menu.h"
#include "pomdog/experimental/gui/context_menu_item.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/scroll_view.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/experimental/gui/vertical_layout.h"
#include "pomdog/math/math.h"

namespace pomdog::gui {

ContextMenu::ContextMenu(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    int widthIn,
    int heightIn)
    : Widget(dispatcher)
    , padding{8, 1, 8, 1}
    , needToUpdateLayout(true)
{
    SetSize(widthIn, heightIn);

    verticalLayout = std::make_shared<VerticalLayout>(dispatcher, 140, 10);
    verticalLayout->SetStackedLayout(true);
    verticalLayout->SetLayoutSpacing(0);

    scrollView = std::make_shared<gui::ScrollView>(dispatcher, 140, 100);
    scrollView->SetWidget(verticalLayout);

    SetHierarchySortOrder(HierarchySortOrder::Front);
}

void ContextMenu::SetPosition(const Point2D& positionIn)
{
    Widget::SetPosition(positionIn);

    POMDOG_ASSERT(scrollView != nullptr);
    scrollView->MarkParentTransformDirty();
}

void ContextMenu::SetPadding(const Thickness& paddingIn)
{
    padding = paddingIn;
    needToUpdateLayout = true;
}

void ContextMenu::MarkParentTransformDirty()
{
    Widget::MarkParentTransformDirty();

    POMDOG_ASSERT(scrollView);
    scrollView->MarkParentTransformDirty();
}

void ContextMenu::MarkContentLayoutDirty()
{
    needToUpdateLayout = true;
}

bool ContextMenu::GetSizeToFitContent() const noexcept
{
    return false;
}

void ContextMenu::ScrollToTop()
{
    POMDOG_ASSERT(scrollView);
    scrollView->ScrollToTop();
}

void ContextMenu::ScrollToEnd()
{
    POMDOG_ASSERT(scrollView);
    scrollView->ScrollToEnd();
}

void ContextMenu::ScrollTo(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(scrollView);
    scrollView->ScrollTo(widget);
}

void ContextMenu::OnEnter()
{
    POMDOG_ASSERT(scrollView != nullptr);
    scrollView->MarkParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    scrollView->SetParent(shared_from_this());
    scrollView->OnEnter();
}

void ContextMenu::AddChild(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(verticalLayout);
    verticalLayout->AddChild(widget);
    needToUpdateLayout = true;
}

std::shared_ptr<Widget> ContextMenu::GetChildAt(const Point2D& position)
{
    POMDOG_ASSERT(scrollView != nullptr);
    auto bounds = scrollView->GetBounds();
    if (bounds.contains(position)) {
        return scrollView;
    }
    return nullptr;
}

void ContextMenu::UpdateAnimation(const Duration& frameDuration)
{
    POMDOG_ASSERT(scrollView != nullptr);
    scrollView->UpdateAnimation(frameDuration);
}

void ContextMenu::UpdateLayout()
{
    POMDOG_ASSERT(scrollView);
    scrollView->DoLayout();

    if (!needToUpdateLayout) {
        return;
    }

    const auto scrollViewHeight = std::max(30, GetHeight() - (padding.top + padding.bottom));

    if (scrollViewHeight != scrollView->GetHeight()) {
        scrollView->SetSize(scrollView->GetWidth(), scrollViewHeight);
        scrollView->DoLayout();
    }

    const auto requiredHeight = padding.top + scrollView->GetHeight() + padding.bottom;
    if (requiredHeight != GetHeight()) {
        // NOTE: Keeping the original position
        const auto positionOffset = Point2D{0, GetHeight() - requiredHeight};
        SetPosition(GetPosition() + positionOffset);

        // NOTE: Resizing this panel
        SetSize(GetWidth(), requiredHeight);

        auto parent = GetParent();
        if (parent) {
            parent->MarkContentLayoutDirty();
        }
    }

    // NOTE: Update layout for children
    {
        scrollView->SetPosition(Point2D{padding.left, padding.bottom});

        switch (scrollView->GetHorizontalAlignment()) {
        case HorizontalAlignment::Stretch: {
            auto childWidth = GetWidth() - (padding.left + padding.right);
            scrollView->SetSize(childWidth, scrollView->GetHeight());
            scrollView->MarkContentLayoutDirty();
            break;
        }
        case HorizontalAlignment::Left:
            break;
        case HorizontalAlignment::Right:
            break;
        }
    }

    needToUpdateLayout = false;
}

void ContextMenu::DoLayout()
{
    UpdateLayout();
}

void ContextMenu::Draw(DrawingContext& drawingContext)
{
    const auto* colorScheme = drawingContext.GetColorScheme();
    POMDOG_ASSERT(colorScheme != nullptr);

    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());

    auto primitiveBatch = drawingContext.GetPrimitiveBatch();

    const auto w = static_cast<float>(GetWidth());
    const auto h = static_cast<float>(GetHeight());

    primitiveBatch->drawRectangle(
        Matrix3x2::createIdentity(),
        math::toVector2(globalPos) + Vector2{-4.0f, -4.0f},
        w + 8.0f,
        h + 7.0f,
        colorScheme->ContextMenuShadowColor);

    primitiveBatch->drawRectangle(
        Matrix3x2::createIdentity(),
        math::toVector2(globalPos) + Vector2{-1.5f, -1.5f},
        w + 3.0f,
        h + 3.0f,
        colorScheme->ContextMenuShadowColor);

    primitiveBatch->drawRectangle(
        Rectangle{globalPos.x - 1, globalPos.y - 1, GetWidth() + 2, GetHeight() + 2},
        colorScheme->ContextMenuOutlineColor);

    primitiveBatch->drawRectangle(
        Rectangle{globalPos.x, globalPos.y, GetWidth(), GetHeight()},
        colorScheme->ContextMenuBorderColor);

    primitiveBatch->drawRectangle(
        Rectangle{globalPos.x + 1, globalPos.y + 1, GetWidth() - 2, GetHeight() - 2},
        colorScheme->ContextMenuBackgroundColor);

    primitiveBatch->flush();

    drawingContext.PushTransform(globalPos);

    POMDOG_ASSERT(scrollView);
    scrollView->Draw(drawingContext);

    drawingContext.PopTransform();
}

} // namespace pomdog::gui
