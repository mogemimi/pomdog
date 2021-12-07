// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/horizontal_layout.hpp"
#include "pomdog/experimental/gui/drawing_context.hpp"
#include "pomdog/experimental/gui/pointer_point.hpp"
#include "pomdog/experimental/gui/ui_event_dispatcher.hpp"
#include "pomdog/experimental/gui/ui_helper.hpp"

namespace Pomdog::GUI {

HorizontalLayout::HorizontalLayout(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    int widthIn,
    int heightIn)
    : Widget(dispatcher)
    , margin{0, 0, 0, 0}
    , layoutSpacing(2)
    , needToUpdateLayout(true)
    , isStackedLayout(false)
{
    SetSize(widthIn, heightIn);
    SetInteractable(false);
}

void HorizontalLayout::SetMargin(const Thickness& marginIn)
{
    margin = marginIn;
    needToUpdateLayout = true;
}

void HorizontalLayout::SetLayoutSpacing(int spacingIn)
{
    layoutSpacing = spacingIn;
    needToUpdateLayout = true;
}

void HorizontalLayout::SetStackedLayout(bool isStackedLayoutIn)
{
    if (isStackedLayout == isStackedLayoutIn) {
        return;
    }

    isStackedLayout = isStackedLayoutIn;
    needToUpdateLayout = true;
}

void HorizontalLayout::SetPosition(const Point2D& positionIn)
{
    Widget::SetPosition(positionIn);

    for (auto& child : children) {
        POMDOG_ASSERT(child.widget != nullptr);
        child.widget->MarkParentTransformDirty();
    }
}

void HorizontalLayout::MarkParentTransformDirty()
{
    Widget::MarkParentTransformDirty();
    for (auto& child : children) {
        POMDOG_ASSERT(child.widget != nullptr);
        child.widget->MarkParentTransformDirty();
    }
}

void HorizontalLayout::MarkContentLayoutDirty()
{
    needToUpdateLayout = true;
}

bool HorizontalLayout::GetSizeToFitContent() const noexcept
{
    return false;
}

void HorizontalLayout::OnEnter()
{
}

void HorizontalLayout::AddChild(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(widget->GetParent() == nullptr);

    Item item;
    item.widget = widget;
    item.stretch = 0;
    children.push_back(std::move(item));

    widget->MarkParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    widget->SetParent(shared_from_this());
    widget->OnEnter();

    needToUpdateLayout = true;
}

std::shared_ptr<Widget> HorizontalLayout::GetChildAt(const Point2D& position)
{
    for (auto& child : children) {
        POMDOG_ASSERT(child.widget != nullptr);
        auto bounds = child.widget->GetBounds();
        if (bounds.Contains(position)) {
            return child.widget;
        }
    }
    return nullptr;
}

void HorizontalLayout::UpdateAnimation(const Duration& frameDuration)
{
    for (auto& child : children) {
        POMDOG_ASSERT(child.widget != nullptr);
        child.widget->UpdateAnimation(frameDuration);
    }
}

void HorizontalLayout::SetStretchFactor(int index, int stretch)
{
    POMDOG_ASSERT(stretch >= 0);
    POMDOG_ASSERT(!children.empty());

    if (index < 0) {
        return;
    }
    if (index < static_cast<int>(children.size())) {
        auto& child = children[static_cast<size_t>(index)];
        child.stretch = stretch;
    }
}

void HorizontalLayout::SetStretchFactor(const std::shared_ptr<Widget>& widget, int stretch)
{
    POMDOG_ASSERT(stretch >= 0);
    POMDOG_ASSERT(!children.empty());
    POMDOG_ASSERT(widget->GetParent() == shared_from_this());

    if (children.empty()) {
        return;
    }
    if (widget->GetParent() != shared_from_this()) {
        return;
    }

    auto iter = std::find_if(
        std::begin(children),
        std::end(children),
        [&](const auto& c) -> bool { return c.widget == widget; });

    if (iter != std::end(children)) {
        iter->stretch = stretch;
    }
}

void HorizontalLayout::UpdateLayout()
{
    bool hasStretchFactor = false;
    int totalStretchFactor = 0;
    for (auto& child : children) {
        child.widget->DoLayout();
        if (child.stretch > 1) {
            hasStretchFactor = true;
        }
        totalStretchFactor += std::max(1, child.stretch);
    }

    if (!needToUpdateLayout) {
        return;
    }

    {
        int maxHeight = 0;
        for (auto& child : children) {
            maxHeight = std::max(child.widget->GetHeight(), maxHeight);
        }

        const auto requiredHeight = maxHeight + margin.Top + margin.Bottom;
        if (requiredHeight > GetHeight()) {
            // NOTE: Resizing this panel
            SetSize(GetWidth(), requiredHeight);

            auto parent = GetParent();
            if (parent) {
                parent->MarkContentLayoutDirty();
            }
        }
    }

    if (children.empty()) {
        needToUpdateLayout = false;
        return;
    }

    POMDOG_ASSERT(!children.empty());
    const auto lineSpacingAll = (layoutSpacing * std::max(0, static_cast<int>(children.size()) - 1));
    const auto horizontalSpacing = (margin.Left + margin.Right + lineSpacingAll);
    const auto containerWidth = std::max(1, (GetWidth() - horizontalSpacing));
    const auto cellWidth = [&]() -> int {
        if (!isStackedLayout) {
            return containerWidth / static_cast<int>(children.size());
        }

        int childrenCount = 0;
        int fixedWidth = 0;
        for (auto& child : children) {
            if (child.widget->GetHorizontalAlignment() == HorizontalAlignment::Stretch) {
                ++childrenCount;
            }
            else {
                fixedWidth += child.widget->GetWidth();
            }
        }

        POMDOG_ASSERT(childrenCount >= 0);
        if (childrenCount == 0) {
            return std::max(1, (GetWidth() - horizontalSpacing) / static_cast<int>(children.size()));
        }
        return (GetWidth() - fixedWidth) / childrenCount;
    }();

    int offsetX = margin.Left;
    double inverseStretchFactor = 1.0 / totalStretchFactor;

    auto calcWidth = [&](int stretch) -> int {
        if (!hasStretchFactor) {
            return cellWidth;
        }
        return static_cast<int>(static_cast<double>(containerWidth * std::max(1, stretch)) * inverseStretchFactor);
    };

    // NOTE: Update layout for children
    for (auto& child : children) {
        POMDOG_ASSERT(child.widget != nullptr);
        auto position = Point2D{offsetX, 0};

        switch (child.widget->GetHorizontalAlignment()) {
        case HorizontalAlignment::Stretch:
            child.widget->SetSize(calcWidth(child.stretch), child.widget->GetHeight());
            child.widget->MarkContentLayoutDirty();
            break;
        case HorizontalAlignment::Right:
            if (!isStackedLayout) {
                position.X = offsetX + cellWidth - child.widget->GetWidth();
            }
            break;
        case HorizontalAlignment::Left:
            break;
        }

        if (child.widget->GetHeight() < GetHeight()) {
            position.Y = (GetHeight() - child.widget->GetHeight()) / 2;
        }
        else {
            position.Y = margin.Bottom;
        }

        child.widget->SetPosition(position);
        if (isStackedLayout) {
            offsetX += child.widget->GetWidth();
        }
        else {
            offsetX += calcWidth(child.stretch);
        }
        offsetX += layoutSpacing;
    }

    needToUpdateLayout = false;
}

void HorizontalLayout::DoLayout()
{
    UpdateLayout();
}

void HorizontalLayout::Draw(DrawingContext& drawingContext)
{
    UpdateLayout();
    POMDOG_ASSERT(!needToUpdateLayout);

    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());
    drawingContext.PushTransform(globalPos);

    for (auto& child : children) {
        POMDOG_ASSERT(child.widget != nullptr);
        child.widget->Draw(drawingContext);
    }

    drawingContext.PopTransform();
}

} // namespace Pomdog::GUI
