// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/GUI/ListViewItem.hpp"
#include "Pomdog/Experimental/GUI/DrawingContext.hpp"
#include "Pomdog/Experimental/GUI/ListView.hpp"
#include "Pomdog/Experimental/GUI/PointerPoint.hpp"
#include "Pomdog/Experimental/GUI/UIEventDispatcher.hpp"
#include "Pomdog/Experimental/GUI/UIHelper.hpp"
#include "Pomdog/Math/Math.hpp"

namespace Pomdog::GUI {

ListViewItem::ListViewItem(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , listViewIndex(0)
    , marginLeft(10)
    , marginRight(10)
    , horizontalAlignment(HorizontalAlignment::Stretch)
    , verticalAlignment(VerticalAlignment::Top)
    , isEnabled(true)
    , isHovered(false)
    , isFocused(false)
    , isSelected(false)
{
    SetSize(74, 20);
}

bool ListViewItem::IsEnabled() const noexcept
{
    return isEnabled;
}

void ListViewItem::SetEnabled(bool isEnabledIn)
{
    this->isEnabled = isEnabledIn;
}

bool ListViewItem::IsHovered() const noexcept
{
    return isHovered;
}

bool ListViewItem::IsFocused() const noexcept
{
    return isFocused;
}

bool ListViewItem::IsSelected() const noexcept
{
    return isSelected;
}

void ListViewItem::SetSelected(bool selected)
{
    isSelected = selected;
}

int ListViewItem::GetListViewIndex() const noexcept
{
    return listViewIndex;
}

void ListViewItem::SetListViewIndex(int index) noexcept
{
    listViewIndex = static_cast<std::int32_t>(index);
}

void ListViewItem::SetWidget(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(widget != nullptr);
    POMDOG_ASSERT(!widget->GetParent());

    child = widget;
    child->MarkParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    child->SetParent(shared_from_this());
    child->OnEnter();

    constexpr auto marginBottom = 6;
    child->SetPosition(Point2D{marginLeft, marginBottom});
}

void ListViewItem::SetMargin(const Thickness& margin)
{
    static_assert(std::is_same_v<decltype(marginLeft), std::int16_t>);
    static_assert(std::is_same_v<decltype(marginRight), std::int16_t>);

    marginLeft = static_cast<std::int16_t>(margin.Left);
    marginRight = static_cast<std::int16_t>(margin.Right);
}

void ListViewItem::SetHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    this->horizontalAlignment = horizontalAlignmentIn;
}

void ListViewItem::SetVerticalAlignment(VerticalAlignment verticalAlignmentIn) noexcept
{
    this->verticalAlignment = verticalAlignmentIn;
}

HorizontalAlignment ListViewItem::GetHorizontalAlignment() const noexcept
{
    return horizontalAlignment;
}

VerticalAlignment ListViewItem::GetVerticalAlignment() const noexcept
{
    return verticalAlignment;
}

void ListViewItem::OnEnter()
{
}

void ListViewItem::OnFocusIn()
{
    isFocused = true;
}

void ListViewItem::OnFocusOut()
{
    isFocused = false;
}

void ListViewItem::OnPointerEntered([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }
    isHovered = true;
}

void ListViewItem::OnPointerExited([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isHovered = false;
}

void ListViewItem::OnPointerPressed([[maybe_unused]] const PointerPoint& pointerPoint)
{
}

void ListViewItem::OnPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }
    isSelected = true;

    auto parent = GetParent();
    POMDOG_ASSERT(parent);
    POMDOG_ASSERT(std::dynamic_pointer_cast<VerticalLayout>(parent) != nullptr);

    auto listView = std::static_pointer_cast<ListView>(parent->GetParent());
    POMDOG_ASSERT(std::dynamic_pointer_cast<ListView>(parent->GetParent()) != nullptr);
    POMDOG_ASSERT(listView);

    listView->SetCurrentIndex(listViewIndex);
}

std::shared_ptr<Widget> ListViewItem::GetChildAt(const Point2D& position)
{
    if (child != nullptr) {
        auto bounds = child->GetBounds();
        if (bounds.Contains(position)) {
            return child;
        }
    }
    return nullptr;
}

std::shared_ptr<Widget> ListViewItem::GetChild()
{
    return child;
}

void ListViewItem::UpdateAnimation(const Duration& frameDuration)
{
    if (child != nullptr) {
        child->UpdateAnimation(frameDuration);
    }
}

void ListViewItem::DoLayout()
{
    if (child != nullptr) {
        child->DoLayout();
    }
}

void ListViewItem::Draw(DrawingContext& drawingContext)
{
    const auto* colorScheme = drawingContext.GetColorScheme();
    POMDOG_ASSERT(colorScheme != nullptr);

    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());
    auto primitiveBatch = drawingContext.GetPrimitiveBatch();

    if (isEnabled && (isHovered || isSelected)) {
        auto color = colorScheme->ListViewItemColorHovered;

        if (isSelected) {
            if (isFocused) {
                color = colorScheme->ListViewItemColorActive;
            }
            else {
                color = colorScheme->ListViewItemColorSelected;
            }
        }

        primitiveBatch->DrawRectangle(
            Rectangle{globalPos.X, globalPos.Y, GetWidth(), GetHeight()},
            color);

        primitiveBatch->Flush();
    }

    if (child != nullptr) {
        drawingContext.PushTransform(globalPos);

        if (child != nullptr) {
            child->Draw(drawingContext);
        }

        drawingContext.PopTransform();
    }
}

} // namespace Pomdog::GUI
