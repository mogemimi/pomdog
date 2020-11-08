// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/GUI/ListView.hpp"
#include "Pomdog/Experimental/GUI/DrawingContext.hpp"
#include "Pomdog/Experimental/GUI/ListViewItem.hpp"
#include "Pomdog/Experimental/GUI/PointerPoint.hpp"
#include "Pomdog/Experimental/GUI/ScrollView.hpp"
#include "Pomdog/Experimental/GUI/UIEventDispatcher.hpp"
#include "Pomdog/Experimental/GUI/UIHelper.hpp"
#include "Pomdog/Experimental/GUI/VerticalLayout.hpp"
#include "Pomdog/Math/Math.hpp"

namespace Pomdog::GUI {

ListView::ListView(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    int widthIn,
    int heightIn)
    : Widget(dispatcher)
    , padding{4, 1, 4, 1}
    , needToUpdateLayout(true)
{
    SetSize(widthIn, heightIn);

    verticalLayout = std::make_shared<VerticalLayout>(dispatcher, 140, 10);
    verticalLayout->SetStackedLayout(true);
    verticalLayout->SetLayoutSpacing(0);

    SetHierarchySortOrder(HierarchySortOrder::Front);
}

void ListView::SetPosition(const Point2D& positionIn)
{
    Widget::SetPosition(positionIn);

    POMDOG_ASSERT(verticalLayout != nullptr);
    verticalLayout->MarkParentTransformDirty();
}

void ListView::SetPadding(const Thickness& paddingIn)
{
    padding = paddingIn;
    needToUpdateLayout = true;
}

void ListView::MarkParentTransformDirty()
{
    Widget::MarkParentTransformDirty();

    POMDOG_ASSERT(verticalLayout);
    verticalLayout->MarkParentTransformDirty();
}

void ListView::MarkContentLayoutDirty()
{
    needToUpdateLayout = true;
}

bool ListView::GetSizeToFitContent() const noexcept
{
    return false;
}

void ListView::OnEnter()
{
    POMDOG_ASSERT(verticalLayout != nullptr);
    verticalLayout->MarkParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    verticalLayout->SetParent(shared_from_this());
    verticalLayout->OnEnter();
}

void ListView::AddChild(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(verticalLayout != nullptr);

    auto itemCount = verticalLayout->GetItemCount();

    auto dispatcher = GetDispatcher();
    auto item = std::make_shared<ListViewItem>(dispatcher);
    item->SetWidget(widget);
    item->SetListViewIndex(itemCount);

    verticalLayout->AddChild(item);
    needToUpdateLayout = true;
}

void ListView::RemoveChild(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(verticalLayout != nullptr);
    verticalLayout->RemoveChild(widget);

    // NOTE: Reindexing
    const auto itemCount = verticalLayout->GetItemCount();
    for (int i = 0; i < itemCount; i++) {
        auto item = std::static_pointer_cast<ListViewItem>(verticalLayout->GetChildAt(i));
        item->SetListViewIndex(i);
    }

    needToUpdateLayout = true;
}

void ListView::RemoveChild(int index)
{
    POMDOG_ASSERT(verticalLayout != nullptr);
    verticalLayout->RemoveChild(index);

    // NOTE: Reindexing
    const auto itemCount = verticalLayout->GetItemCount();
    for (int i = 0; i < itemCount; i++) {
        auto item = std::static_pointer_cast<ListViewItem>(verticalLayout->GetChildAt(i));
        item->SetListViewIndex(i);
    }

    needToUpdateLayout = true;
}

int ListView::GetItemCount() const noexcept
{
    POMDOG_ASSERT(verticalLayout != nullptr);
    return verticalLayout->GetItemCount();
}

std::optional<int> ListView::GetCurrentIndex() const noexcept
{
    return currentIndex;
}

void ListView::SetCurrentIndex(int index)
{
    POMDOG_ASSERT(verticalLayout != nullptr);

    if ((currentIndex != std::nullopt) &&
        (index != *currentIndex) &&
        (*currentIndex < verticalLayout->GetItemCount())) {
        if (auto child = verticalLayout->GetChildAt(*currentIndex); child != nullptr) {
            auto deselectedItem = std::dynamic_pointer_cast<ListViewItem>(child);
            POMDOG_ASSERT(deselectedItem);

            deselectedItem->SetSelected(false);
        }
    }

    auto child = verticalLayout->GetChildAt(index);
    if (child == nullptr) {
        currentIndex = std::nullopt;
        return;
    }

    auto selectedItem = std::dynamic_pointer_cast<ListViewItem>(child);
    POMDOG_ASSERT(selectedItem);
    selectedItem->SetSelected(true);

    currentIndex = index;
    CurrentIndexChanged(currentIndex);
}

std::shared_ptr<Widget> ListView::GetChildAt(const Point2D& position)
{
    POMDOG_ASSERT(verticalLayout != nullptr);
    auto bounds = verticalLayout->GetBounds();
    if (bounds.Contains(position)) {
        return verticalLayout;
    }
    return nullptr;
}

std::shared_ptr<Widget> ListView::GetChildAt(int index)
{
    POMDOG_ASSERT(verticalLayout != nullptr);
    auto item = std::static_pointer_cast<ListViewItem>(verticalLayout->GetChildAt(index));
    return item->GetChild();
}

void ListView::UpdateAnimation(const Duration& frameDuration)
{
    POMDOG_ASSERT(verticalLayout != nullptr);
    verticalLayout->UpdateAnimation(frameDuration);
}

void ListView::UpdateLayout()
{
    POMDOG_ASSERT(verticalLayout != nullptr);
    verticalLayout->DoLayout();

    if (!needToUpdateLayout) {
        return;
    }

    const auto requiredHeight = padding.Top + verticalLayout->GetHeight() + padding.Bottom;
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
        verticalLayout->SetPosition(Point2D{padding.Left, padding.Bottom});

        switch (verticalLayout->GetHorizontalAlignment()) {
        case HorizontalAlignment::Stretch: {
            auto childWidth = GetWidth() - (padding.Left + padding.Right);
            verticalLayout->SetSize(childWidth, verticalLayout->GetHeight());
            verticalLayout->MarkContentLayoutDirty();
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

void ListView::DoLayout()
{
    UpdateLayout();
}

void ListView::Draw(DrawingContext& drawingContext)
{
    const auto* colorScheme = drawingContext.GetColorScheme();
    POMDOG_ASSERT(colorScheme != nullptr);

    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());

    auto primitiveBatch = drawingContext.GetPrimitiveBatch();

    if (colorScheme->ListViewBackgroundColorOdd.A > 0) {
        primitiveBatch->DrawRectangle(
            Rectangle{globalPos.X, globalPos.Y, GetWidth(), GetHeight()},
            colorScheme->ListViewBackgroundColorOdd);
    }

    if (colorScheme->ListViewBackgroundColorEven.A > 0) {
        const auto itemCount = verticalLayout->GetItemCount();
        for (int i = 0; i < itemCount; i++) {
            if (i % 2 == 0) {
                continue;
            }
            auto child = verticalLayout->GetChildAt(i);
            auto childPos = child->GetGlobalPosition();

            primitiveBatch->DrawRectangle(
                Rectangle{globalPos.X, childPos.Y, GetWidth(), child->GetHeight()},
                colorScheme->ListViewBackgroundColorEven);
        }
    }

    primitiveBatch->Flush();

    drawingContext.PushTransform(globalPos);

    POMDOG_ASSERT(verticalLayout);
    verticalLayout->Draw(drawingContext);

    drawingContext.PopTransform();
}

} // namespace Pomdog::GUI
