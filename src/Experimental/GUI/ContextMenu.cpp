// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/GUI/ContextMenu.hpp"
#include "Pomdog/Experimental/GUI/ContextMenuItem.hpp"
#include "Pomdog/Experimental/GUI/DrawingContext.hpp"
#include "Pomdog/Experimental/GUI/PointerPoint.hpp"
#include "Pomdog/Experimental/GUI/ScrollView.hpp"
#include "Pomdog/Experimental/GUI/UIEventDispatcher.hpp"
#include "Pomdog/Experimental/GUI/UIHelper.hpp"
#include "Pomdog/Experimental/GUI/VerticalLayout.hpp"
#include "Pomdog/Math/MathHelper.hpp"

namespace Pomdog::GUI {

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

    scrollView = std::make_shared<GUI::ScrollView>(dispatcher, 140, 100);
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
    if (bounds.Contains(position)) {
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

    const auto scrollViewHeight = std::max(30, GetHeight() - (padding.Top + padding.Bottom));

    if (scrollViewHeight != scrollView->GetHeight()) {
        scrollView->SetSize(scrollView->GetWidth(), scrollViewHeight);
        scrollView->DoLayout();
    }

    const auto requiredHeight = padding.Top + scrollView->GetHeight() + padding.Bottom;
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
        scrollView->SetPosition(Point2D{padding.Left, padding.Bottom});

        switch (scrollView->GetHorizontalAlignment()) {
        case HorizontalAlignment::Stretch: {
            auto childWidth = GetWidth() - (padding.Left + padding.Right);
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

    primitiveBatch->DrawRectangle(
        Matrix3x2::Identity,
        MathHelper::ToVector2(globalPos) + Vector2{-4.0f, -4.0f},
        w + 8.0f,
        h + 7.0f,
        colorScheme->ContextMenuShadowColor);

    primitiveBatch->DrawRectangle(
        Matrix3x2::Identity,
        MathHelper::ToVector2(globalPos) + Vector2{-1.5f, -1.5f},
        w + 3.0f,
        h + 3.0f,
        colorScheme->ContextMenuShadowColor);

    primitiveBatch->DrawRectangle(
        Rectangle{globalPos.X - 1, globalPos.Y - 1, GetWidth() + 2, GetHeight() + 2},
        colorScheme->ContextMenuOutlineColor);

    primitiveBatch->DrawRectangle(
        Rectangle{globalPos.X, globalPos.Y, GetWidth(), GetHeight()},
        colorScheme->ContextMenuBorderColor);

    primitiveBatch->DrawRectangle(
        Rectangle{globalPos.X + 1, globalPos.Y + 1, GetWidth() - 2, GetHeight() - 2},
        colorScheme->ContextMenuBackgroundColor);

    primitiveBatch->Flush();

    drawingContext.PushTransform(globalPos);

    POMDOG_ASSERT(scrollView);
    scrollView->Draw(drawingContext);

    drawingContext.PopTransform();
}

} // namespace Pomdog::GUI
