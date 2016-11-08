// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/UI/TreeView.hpp"
#include "Pomdog.Experimental/UI/DrawingContext.hpp"
#include "Pomdog.Experimental/UI/PointerPoint.hpp"
#include "Pomdog.Experimental/UI/UIHelper.hpp"
#include "Pomdog.Experimental/UI/UIEventDispatcher.hpp"
#include "Pomdog.Experimental/UI/FontSize.hpp"
#include "Pomdog.Experimental/UI/FontWeight.hpp"

namespace Pomdog {
namespace UI {

TreeView::TreeView(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : UIElement(dispatcher)
    , layoutSpacing(6)
    , indentation(14)
    , expanded(true)
    , needToUpdateLayout(true)
{
    SetSize(50, 12);

    horizontalLayout = std::make_shared<UI::HorizontalLayout>(dispatcher, 140, 10);
    horizontalLayout->SetStackedLayout(true);
    horizontalLayout->SetLayoutSpacing(5);

    disclosureTriangleButton = std::make_shared<UI::DisclosureTriangleButton>(dispatcher);
    horizontalLayout->AddChild(disclosureTriangleButton);

    textBlock = std::make_shared<UI::TextBlock>(dispatcher);
    textBlock->SetFontWeight(UI::FontWeight::Bold);
    textBlock->SetColor(Color{252, 251, 248, 255});
    horizontalLayout->AddChild(textBlock);

    verticalLayout = std::make_shared<UI::VerticalLayout>(dispatcher, 140, 10);
    verticalLayout->SetLayoutSpacing(8);
    verticalLayout->SetMargin(Thickness{2, 0, 4, 0});

    connection = disclosureTriangleButton->Toggled.Connect([this](bool isOn) {
        this->SetExpanded(isOn);
    });
}

void TreeView::SetText(const std::string& text)
{
    textBlock->SetText(text);
}

void TreeView::SetExpanded(bool expandedIn)
{
    if (expanded == expandedIn) {
        return;
    }
    expanded = expandedIn;
    needToUpdateLayout = true;
}

void TreeView::SetVerticalLayoutSpacing(int spacing)
{
    POMDOG_ASSERT(verticalLayout);
    verticalLayout->SetLayoutSpacing(spacing);
}

void TreeView::OnEnter()
{
    std::vector<std::shared_ptr<UI::UIElement>> children;
    children.push_back(horizontalLayout);
    children.push_back(verticalLayout);

    for (auto & child : children) {
        POMDOG_ASSERT(child);
        child->MarkParentDrawOrderDirty();
        child->MarkParentTransformDirty();

        POMDOG_ASSERT(shared_from_this());
        child->SetParent(shared_from_this());
        child->OnEnter();
    }
}

HorizontalAlignment TreeView::GetHorizontalAlignment() const noexcept
{
    return UI::HorizontalAlignment::Stretch;
}

VerticalAlignment TreeView::GetVerticalAlignment() const noexcept
{
    return UI::VerticalAlignment::Top;
}

void TreeView::SetTransform(const Matrix3x2& transformMatrixIn)
{
    UIElement::SetTransform(transformMatrixIn);
    verticalLayout->MarkParentTransformDirty();
    horizontalLayout->MarkParentTransformDirty();
}

void TreeView::MarkParentTransformDirty()
{
    UIElement::MarkParentTransformDirty();
    verticalLayout->MarkParentTransformDirty();
    horizontalLayout->MarkParentTransformDirty();
}

void TreeView::MarkContentLayoutDirty()
{
    needToUpdateLayout = true;
}

void TreeView::AddChild(const std::shared_ptr<UIElement>& element)
{
    POMDOG_ASSERT(verticalLayout);
    verticalLayout->AddChild(element);
}

void TreeView::UpdateLayout()
{
    verticalLayout->DoLayout();
    horizontalLayout->DoLayout();

    if (!needToUpdateLayout) {
        return;
    }

    const auto verticalLayoutHeight = verticalLayout->GetHeight();
    const auto horizontalLayoutHeight = horizontalLayout->GetHeight();

    {
        auto requiredHeight = horizontalLayoutHeight;
        if (expanded) {
            requiredHeight += layoutSpacing;
            requiredHeight += verticalLayoutHeight;
        }

        if (requiredHeight != GetHeight()) {
            // NOTE: Resizing this panel
            SetSize(std::max(GetWidth(), indentation + 1), requiredHeight);
            if (auto parent = GetParent()) {
                parent->MarkContentLayoutDirty();
            }
        }
    }
    {
        Vector2 position = {0.0f, 0.0f};
        switch (horizontalLayout->GetHorizontalAlignment()) {
        case HorizontalAlignment::Stretch:
            horizontalLayout->SetSize(GetWidth(), horizontalLayout->GetHeight());
            horizontalLayout->MarkContentLayoutDirty();
            break;
        case HorizontalAlignment::Right:
            position.X = static_cast<float>(GetWidth() - horizontalLayout->GetWidth());
            break;
        case HorizontalAlignment::Left:
            break;
        }
        if (expanded) {
            position.Y += layoutSpacing;
            position.Y += verticalLayoutHeight;
        }
        horizontalLayout->SetTransform(Matrix3x2::CreateTranslation(position));
    }
    {
        POMDOG_ASSERT(indentation < GetWidth());

        Vector2 position = {static_cast<float>(indentation), 0.0f};
        switch (verticalLayout->GetHorizontalAlignment()) {
        case HorizontalAlignment::Stretch:
            verticalLayout->SetSize(GetWidth() - indentation, verticalLayout->GetHeight());
            verticalLayout->MarkContentLayoutDirty();
            break;
        case HorizontalAlignment::Right:
            position.X = static_cast<float>(GetWidth() - verticalLayout->GetWidth());
            break;
        case HorizontalAlignment::Left:
            break;
        }
        if (!expanded) {
            // TODO: bad code
            position.X = -1000.0f;
            position.Y = -1000.0f;
        }
        verticalLayout->SetTransform(Matrix3x2::CreateTranslation(position));
    }

    needToUpdateLayout = false;
}

void TreeView::DoLayout()
{
    UpdateLayout();
}

void TreeView::Draw(DrawingContext & drawingContext)
{
    UpdateLayout();

    auto transform = GetTransform() * drawingContext.Top();
    drawingContext.Push(transform);

    horizontalLayout->Draw(drawingContext);

    if (expanded) {
        verticalLayout->Draw(drawingContext);
    }

    drawingContext.Pop();
}

} // namespace UI
} // namespace Pomdog
