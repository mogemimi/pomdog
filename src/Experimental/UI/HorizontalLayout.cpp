// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/UI/HorizontalLayout.hpp"
#include "Pomdog/Experimental/UI/DrawingContext.hpp"
#include "Pomdog/Experimental/UI/PointerPoint.hpp"
#include "Pomdog/Experimental/UI/UIEventDispatcher.hpp"
#include "Pomdog/Experimental/UI/UIHelper.hpp"

namespace Pomdog::UI {

HorizontalLayout::HorizontalLayout(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    int widthIn,
    int heightIn)
    : UIElement(dispatcher)
    , margin{0, 0, 0, 0}
    , layoutSpacing(2)
    , needToUpdateLayout(true)
    , isStackedLayout(false)
{
    SetSize(widthIn, heightIn);
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

void HorizontalLayout::SetTransform(const Matrix3x2& transformMatrixIn)
{
    UIElement::SetTransform(transformMatrixIn);
    for (auto& child : children) {
        POMDOG_ASSERT(child.element != nullptr);
        child.element->MarkParentTransformDirty();
    }
}

void HorizontalLayout::MarkParentTransformDirty()
{
    UIElement::MarkParentTransformDirty();
    for (auto& child : children) {
        POMDOG_ASSERT(child.element != nullptr);
        child.element->MarkParentTransformDirty();
    }
}

void HorizontalLayout::MarkContentLayoutDirty()
{
    needToUpdateLayout = true;
}

bool HorizontalLayout::SizeToFitContent() const
{
    return false;
}

void HorizontalLayout::OnEnter()
{
    auto dispatcher = GetDispatcher();
    connection = dispatcher->Connect(shared_from_this());
}

void HorizontalLayout::AddChild(const std::shared_ptr<UIElement>& element)
{
    POMDOG_ASSERT(element->GetParent() == nullptr);

    Item item;
    item.element = element;
    item.stretch = 0;
    children.push_back(std::move(item));

    element->MarkParentDrawOrderDirty();
    element->MarkParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    element->SetParent(shared_from_this());
    element->OnEnter();

    needToUpdateLayout = true;
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

void HorizontalLayout::SetStretchFactor(const std::shared_ptr<UIElement>& element, int stretch)
{
    POMDOG_ASSERT(stretch >= 0);
    POMDOG_ASSERT(!children.empty());
    POMDOG_ASSERT(element->GetParent() == shared_from_this());

    if (children.empty()) {
        return;
    }
    if (element->GetParent() != shared_from_this()) {
        return;
    }

    auto iter = std::find_if(
        std::begin(children),
        std::end(children),
        [&](const auto& c) -> bool { return c.element == element; });

    if (iter != std::end(children)) {
        iter->stretch = stretch;
    }
}

void HorizontalLayout::UpdateLayout()
{
    bool hasStretchFactor = false;
    int totalStretchFactor = 0;
    for (auto& child : children) {
        child.element->DoLayout();
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
            maxHeight = std::max(child.element->GetHeight(), maxHeight);
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
            if (child.element->GetHorizontalAlignment() == HorizontalAlignment::Stretch) {
                ++childrenCount;
            }
            else {
                fixedWidth += child.element->GetWidth();
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
        POMDOG_ASSERT(child.element != nullptr);
        Vector2 position = {static_cast<float>(offsetX), 0.0f};

        switch (child.element->GetHorizontalAlignment()) {
        case HorizontalAlignment::Stretch:
            child.element->SetSize(calcWidth(child.stretch), child.element->GetHeight());
            child.element->MarkContentLayoutDirty();
            break;
        case HorizontalAlignment::Right:
            if (!isStackedLayout) {
                position.X = static_cast<float>(offsetX + cellWidth - child.element->GetWidth());
            }
            break;
        case HorizontalAlignment::Left:
            break;
        }

        if (child.element->GetHeight() < GetHeight()) {
            position.Y = static_cast<float>(GetHeight() - child.element->GetHeight()) * 0.5f;
        }
        else {
            position.Y = static_cast<float>(margin.Bottom);
        }

        child.element->SetTransform(Matrix3x2::CreateTranslation(position));
        if (isStackedLayout) {
            offsetX += child.element->GetWidth();
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

    auto globalTransform = GetTransform() * drawingContext.Top();
    drawingContext.Push(globalTransform);

    for (auto& child : children) {
        POMDOG_ASSERT(child.element != nullptr);
        child.element->Draw(drawingContext);
    }

    drawingContext.Pop();
}

} // namespace Pomdog::UI
