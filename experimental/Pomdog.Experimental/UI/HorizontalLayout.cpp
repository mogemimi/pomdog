// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/UI/HorizontalLayout.hpp"
#include "Pomdog.Experimental/UI/DrawingContext.hpp"
#include "Pomdog.Experimental/UI/PointerPoint.hpp"
#include "Pomdog.Experimental/UI/UIHelper.hpp"
#include "Pomdog.Experimental/UI/UIEventDispatcher.hpp"

namespace Pomdog {
namespace UI {

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
    for (auto & child : children) {
        POMDOG_ASSERT(child);
        child->MarkParentTransformDirty();
    }
}

void HorizontalLayout::MarkParentTransformDirty()
{
    UIElement::MarkParentTransformDirty();
    for (auto & child : children) {
        POMDOG_ASSERT(child);
        child->MarkParentTransformDirty();
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
    POMDOG_ASSERT(!element->GetParent());

    children.push_back(element);

    element->MarkParentDrawOrderDirty();
    element->MarkParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    element->SetParent(shared_from_this());
    element->OnEnter();

    needToUpdateLayout = true;
}

void HorizontalLayout::UpdateLayout()
{
    for (auto & child : children) {
        child->DoLayout();
    }

    if (!needToUpdateLayout) {
        return;
    }

    {
        int maxHeight = 0;
        for (auto & child : children) {
            maxHeight = std::max(child->GetHeight(), maxHeight);
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
    const auto cellWidth = [&]() -> int {
        if (!isStackedLayout) {
            return std::max(1, (GetWidth() - horizontalSpacing) / static_cast<int>(children.size()));
        }

        int childrenCount = 0;
        int fixedWidth = 0;
        for (auto & child : children) {
            if (child->GetHorizontalAlignment() == HorizontalAlignment::Stretch) {
                ++childrenCount;
            }
            else {
                fixedWidth += child->GetWidth();
            }
        }

        POMDOG_ASSERT(childrenCount >= 0);
        if (childrenCount == 0) {
            return std::max(1, (GetWidth() - horizontalSpacing) / static_cast<int>(children.size()));
        }
        return (GetWidth() - fixedWidth) / childrenCount;
    }();

    int offsetX = margin.Left;

    // NOTE: Update layout for children
    for (auto & child : children) {
        POMDOG_ASSERT(child);
        Vector2 position = {static_cast<float>(offsetX), 0.0f};

        switch (child->GetHorizontalAlignment()) {
        case HorizontalAlignment::Stretch:
            child->SetSize(cellWidth, child->GetHeight());
            child->MarkContentLayoutDirty();
            break;
        case HorizontalAlignment::Right:
            if (!isStackedLayout) {
                position.X = static_cast<float>(offsetX + cellWidth - child->GetWidth());
            }
            break;
        case HorizontalAlignment::Left:
            break;
        }

        if (child->GetHeight() < GetHeight()) {
            position.Y = static_cast<float>(GetHeight() - child->GetHeight()) * 0.5f;
        }
        else {
            position.Y = static_cast<float>(margin.Bottom);
        }

        child->SetTransform(Matrix3x2::CreateTranslation(position));
        if (isStackedLayout) {
            offsetX += child->GetWidth();
        }
        else {
            offsetX += cellWidth;
        }
        offsetX += layoutSpacing;
    }

    needToUpdateLayout = false;
}

void HorizontalLayout::DoLayout()
{
    UpdateLayout();
}

void HorizontalLayout::Draw(DrawingContext & drawingContext)
{
    UpdateLayout();
    POMDOG_ASSERT(!needToUpdateLayout);

    auto transform = GetTransform() * drawingContext.Top();

#if 0
    renderCommand.SetInvoker([transform, this](PrimitiveBatch& primitiveBatch) {
        const Color backgroundColor = {45, 45, 48, 225};
        const Color borderColor = {40, 40, 40, 255};
        const Color highlightColor = {106, 106, 106, 255};

        const auto width = static_cast<float>(GetWidth());
        const auto height = static_cast<float>(GetHeight());

        primitiveBatch.DrawRectangle(transform, Vector2::Zero, width, height, backgroundColor);

        primitiveBatch.DrawLine(transform, Vector2{0.0f, 0.0f}, Vector2{width, 0.0f}, borderColor, 1.0f);
        primitiveBatch.DrawLine(transform, Vector2{0.0f, 0.0f}, Vector2{0.0f, height}, borderColor, 1.0f);
        primitiveBatch.DrawLine(transform, Vector2{0.0f, height}, Vector2{width, height}, borderColor, 1.0f);
        primitiveBatch.DrawLine(transform, Vector2{width, 0.0f}, Vector2{width, height}, borderColor, 1.0f);
    });
    drawingContext.PushCommand(renderCommand);
#endif

    drawingContext.Push(transform);

    for (auto & child : children) {
        POMDOG_ASSERT(child);
        child->Draw(drawingContext);
    }

    drawingContext.Pop();
}

} // namespace UI
} // namespace Pomdog
