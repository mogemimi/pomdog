// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/vertical_layout.hpp"
#include "pomdog/experimental/gui/drawing_context.hpp"
#include "pomdog/experimental/gui/pointer_point.hpp"
#include "pomdog/experimental/gui/ui_event_dispatcher.hpp"
#include "pomdog/experimental/gui/ui_helper.hpp"

namespace Pomdog::GUI {

VerticalLayout::VerticalLayout(
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

void VerticalLayout::SetPosition(const Point2D& positionIn)
{
    Widget::SetPosition(positionIn);
    for (auto& child : children) {
        POMDOG_ASSERT(child);
        child->MarkParentTransformDirty();
    }
}

void VerticalLayout::MarkParentTransformDirty()
{
    Widget::MarkParentTransformDirty();
    for (auto& child : children) {
        POMDOG_ASSERT(child);
        child->MarkParentTransformDirty();
    }
}

void VerticalLayout::MarkContentLayoutDirty()
{
    needToUpdateLayout = true;
}

bool VerticalLayout::GetSizeToFitContent() const noexcept
{
    return false;
}

void VerticalLayout::OnEnter()
{
}

void VerticalLayout::AddChild(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(!widget->GetParent());

    children.push_back(widget);

    widget->MarkParentTransformDirty();

    POMDOG_ASSERT(shared_from_this());
    widget->SetParent(shared_from_this());
    widget->OnEnter();

    needToUpdateLayout = true;
}

void VerticalLayout::RemoveChild(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(!widget->GetParent());

    auto iter = std::find(std::begin(children), std::end(children), widget);
    if (iter == std::end(children)) {
        return;
    }
    children.erase(iter);

    needToUpdateLayout = true;
}

void VerticalLayout::RemoveChild(int index)
{
    if (index >= static_cast<int>(children.size())) {
        return;
    }
    auto iter = std::next(std::begin(children), index);
    if (iter == std::end(children)) {
        return;
    }
    children.erase(iter);

    needToUpdateLayout = true;
}

std::shared_ptr<Widget> VerticalLayout::GetChildAt(const Point2D& position)
{
    for (auto& child : children) {
        POMDOG_ASSERT(child != nullptr);
        auto bounds = child->GetBounds();
        if (bounds.Contains(position)) {
            return child;
        }
    }
    return nullptr;
}

std::shared_ptr<Widget> VerticalLayout::GetChildAt(int index)
{
    POMDOG_ASSERT(index < static_cast<int>(children.size()));
    return children.at(static_cast<std::size_t>(index));
}

int VerticalLayout::GetItemCount() const noexcept
{
    return static_cast<int>(children.size());
}

void VerticalLayout::UpdateAnimation(const Duration& frameDuration)
{
    for (auto& child : children) {
        POMDOG_ASSERT(child != nullptr);
        child->UpdateAnimation(frameDuration);
    }
}

void VerticalLayout::UpdateLayout()
{
    for (auto& child : children) {
        child->DoLayout();
    }

    if (!needToUpdateLayout) {
        return;
    }

    if (children.empty()) {
        // NOTE: Resizing this panel
        SetSize(GetWidth(), 0);

        if (auto parent = GetParent(); parent != nullptr) {
            parent->MarkContentLayoutDirty();
        }
        needToUpdateLayout = false;
        return;
    }

    POMDOG_ASSERT(!children.empty());
    const auto lineSpacingAll = (layoutSpacing * std::max(0, static_cast<int>(children.size()) - 1));
    const auto verticalSpacing = (margin.Top + margin.Bottom + lineSpacingAll);

    int requiredHeight = verticalSpacing;
    for (auto& child : children) {
        requiredHeight += child->GetHeight();
    }

    if (requiredHeight != GetHeight()) {
        // NOTE: Resizing this panel
        SetSize(GetWidth(), requiredHeight);

        if (auto parent = GetParent(); parent != nullptr) {
            parent->MarkContentLayoutDirty();
        }
    }

    // NOTE: Update layout for children
    int offsetY = margin.Top;
    for (auto& child : children) {
        offsetY += child->GetHeight();
        auto position = Point2D{margin.Left, GetHeight() - offsetY};

        switch (child->GetHorizontalAlignment()) {
        case HorizontalAlignment::Stretch: {
            auto childWidth = GetWidth() - (margin.Left + margin.Right);
            child->SetSize(childWidth, child->GetHeight());
            child->MarkContentLayoutDirty();
            break;
        }
        case HorizontalAlignment::Right: {
            position.X = GetWidth() - margin.Right - child->GetWidth();
            break;
        }
        case HorizontalAlignment::Left:
            break;
        }

        child->SetPosition(position);
        offsetY += layoutSpacing;
    }

    needToUpdateLayout = false;
}

void VerticalLayout::DoLayout()
{
    UpdateLayout();
}

void VerticalLayout::Draw(DrawingContext& drawingContext)
{
    UpdateLayout();
    POMDOG_ASSERT(!needToUpdateLayout);

    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());
    drawingContext.PushTransform(globalPos);

    for (auto& child : children) {
        POMDOG_ASSERT(child);
        child->Draw(drawingContext);
    }

    drawingContext.PopTransform();
}

} // namespace Pomdog::GUI
