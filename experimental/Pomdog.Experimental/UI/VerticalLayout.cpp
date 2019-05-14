// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/UI/VerticalLayout.hpp"
#include "Pomdog.Experimental/UI/DrawingContext.hpp"
#include "Pomdog.Experimental/UI/PointerPoint.hpp"
#include "Pomdog.Experimental/UI/UIHelper.hpp"
#include "Pomdog.Experimental/UI/UIEventDispatcher.hpp"

namespace Pomdog {
namespace UI {

VerticalLayout::VerticalLayout(
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

void VerticalLayout::SetMargin(const Thickness& marginIn)
{
    margin = marginIn;
    needToUpdateLayout = true;
}

void VerticalLayout::SetLayoutSpacing(int spacingIn)
{
    layoutSpacing = spacingIn;
    needToUpdateLayout = true;
}

void VerticalLayout::SetStackedLayout(bool isStackedLayoutIn)
{
    if (isStackedLayout == isStackedLayoutIn) {
        return;
    }

    isStackedLayout = isStackedLayoutIn;
    needToUpdateLayout = true;
}

void VerticalLayout::SetTransform(const Matrix3x2& transformMatrixIn)
{
    UIElement::SetTransform(transformMatrixIn);
    for (auto & child : children) {
        POMDOG_ASSERT(child);
        child->MarkParentTransformDirty();
    }
}

void VerticalLayout::MarkParentTransformDirty()
{
    UIElement::MarkParentTransformDirty();
    for (auto & child : children) {
        POMDOG_ASSERT(child);
        child->MarkParentTransformDirty();
    }
}

void VerticalLayout::MarkContentLayoutDirty()
{
    needToUpdateLayout = true;
}

bool VerticalLayout::SizeToFitContent() const
{
    return false;
}

void VerticalLayout::OnEnter()
{
    auto dispatcher = GetDispatcher();
    connection = dispatcher->Connect(shared_from_this());
}

void VerticalLayout::AddChild(const std::shared_ptr<UIElement>& element)
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

void VerticalLayout::UpdateLayout()
{
    for (auto & child : children) {
        child->DoLayout();
    }

    if (!needToUpdateLayout) {
        return;
    }

    POMDOG_ASSERT(!children.empty());
    const auto lineSpacingAll = (layoutSpacing * std::max(0, static_cast<int>(children.size()) - 1));
    const auto verticalSpacing = (margin.Top + margin.Bottom + lineSpacingAll);

    int requiredHeight = verticalSpacing;
    for (auto & child : children) {
        requiredHeight += child->GetHeight();
    }

    if (requiredHeight != GetHeight()) {
        // NOTE: Resizing this panel
        SetSize(GetWidth(), requiredHeight);

        auto parent = GetParent();
        if (parent) {
            parent->MarkContentLayoutDirty();
        }
    }

    // NOTE: Update layout for children
    int offsetY = margin.Top;
    for (auto & child : children) {
        offsetY += child->GetHeight();
        Vector2 position = {
            static_cast<float>(margin.Left),
            static_cast<float>(GetHeight() - offsetY)
        };

        switch (child->GetHorizontalAlignment()) {
        case HorizontalAlignment::Stretch: {
            auto childWidth = GetWidth() - (margin.Left + margin.Right);
            child->SetSize(childWidth, child->GetHeight());
            child->MarkContentLayoutDirty();
            break;
        }
        case HorizontalAlignment::Right: {
            position.X = static_cast<float>(GetWidth() - margin.Right - child->GetWidth());
            break;
        }
        case HorizontalAlignment::Left:
            break;
        }

        child->SetTransform(Matrix3x2::CreateTranslation(position));
        offsetY += layoutSpacing;
    }

    needToUpdateLayout = false;
}

void VerticalLayout::DoLayout()
{
    UpdateLayout();
}

void VerticalLayout::Draw(DrawingContext & drawingContext)
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
