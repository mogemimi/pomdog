// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/tree_view.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/gui/drawing_context.hpp"
#include "pomdog/experimental/gui/font_size.hpp"
#include "pomdog/experimental/gui/font_weight.hpp"
#include "pomdog/experimental/gui/pointer_point.hpp"
#include "pomdog/experimental/gui/ui_event_dispatcher.hpp"
#include "pomdog/experimental/gui/ui_helper.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::GUI {

TreeView::TreeView(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , layoutSpacing(6)
    , indentation(14)
    , expanded(true)
    , needToUpdateLayout(true)
{
    SetSize(50, 12);

    horizontalLayout = std::make_shared<HorizontalLayout>(dispatcher, 140, 10);
    horizontalLayout->SetStackedLayout(true);
    horizontalLayout->SetLayoutSpacing(5);

    disclosureTriangleButton = std::make_shared<DisclosureTriangleButton>(dispatcher);
    horizontalLayout->AddChild(disclosureTriangleButton);

    textBlock = std::make_shared<TextBlock>(dispatcher);
    textBlock->SetFontWeight(FontWeight::Bold);
    textBlock->SetColor(Color{252, 251, 248, 255});
    textBlock->SetBaselineHeight(2.0f);
    horizontalLayout->AddChild(textBlock);

    verticalLayout = std::make_shared<VerticalLayout>(dispatcher, 140, 10);
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
    std::array<std::shared_ptr<Widget>, 2> children = {{horizontalLayout, verticalLayout}};

    for (auto& child : children) {
        POMDOG_ASSERT(child != nullptr);
        child->MarkParentTransformDirty();

        POMDOG_ASSERT(shared_from_this());
        child->SetParent(shared_from_this());
        child->OnEnter();
    }
}

HorizontalAlignment TreeView::GetHorizontalAlignment() const noexcept
{
    return HorizontalAlignment::Stretch;
}

VerticalAlignment TreeView::GetVerticalAlignment() const noexcept
{
    return VerticalAlignment::Top;
}

void TreeView::SetPosition(const Point2D& positionIn)
{
    Widget::SetPosition(positionIn);
    verticalLayout->MarkParentTransformDirty();
    horizontalLayout->MarkParentTransformDirty();
}

void TreeView::MarkParentTransformDirty()
{
    Widget::MarkParentTransformDirty();
    verticalLayout->MarkParentTransformDirty();
    horizontalLayout->MarkParentTransformDirty();
}

void TreeView::MarkContentLayoutDirty()
{
    needToUpdateLayout = true;
}

void TreeView::AddChild(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(verticalLayout);
    verticalLayout->AddChild(widget);
}

std::shared_ptr<Widget> TreeView::GetChildAt(const Point2D& position)
{
    std::array<std::shared_ptr<Widget>, 3> children = {{
        horizontalLayout,
        disclosureTriangleButton,
        verticalLayout,
    }};

    if (!expanded) {
        children[2] = nullptr;
    }

    for (auto& child : children) {
        if (child == nullptr) {
            continue;
        }
        auto bounds = child->GetBounds();
        if (bounds.Contains(position)) {
            return child;
        }
    }
    return nullptr;
}

void TreeView::UpdateAnimation(const Duration& frameDuration)
{
    std::array<std::shared_ptr<Widget>, 3> children = {{
        horizontalLayout,
        disclosureTriangleButton,
        verticalLayout,
    }};

    for (auto& child : children) {
        if (child == nullptr) {
            continue;
        }
        child->UpdateAnimation(frameDuration);
    }
}

void TreeView::UpdateLayout()
{
    verticalLayout->DoLayout();
    horizontalLayout->DoLayout();

    if (!needToUpdateLayout) {
        return;
    }

    disclosureTriangleButton->SetOn(expanded);

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
        auto position = Point2D{0, 0};
        switch (horizontalLayout->GetHorizontalAlignment()) {
        case HorizontalAlignment::Stretch:
            horizontalLayout->SetSize(GetWidth(), horizontalLayout->GetHeight());
            horizontalLayout->MarkContentLayoutDirty();
            break;
        case HorizontalAlignment::Right:
            position.X = GetWidth() - horizontalLayout->GetWidth();
            break;
        case HorizontalAlignment::Left:
            break;
        }
        if (expanded) {
            position.Y += layoutSpacing;
            position.Y += verticalLayoutHeight;
        }
        horizontalLayout->SetPosition(position);
    }
    {
        POMDOG_ASSERT(indentation < GetWidth());

        auto position = Point2D{indentation, 0};
        switch (verticalLayout->GetHorizontalAlignment()) {
        case HorizontalAlignment::Stretch:
            verticalLayout->SetSize(GetWidth() - indentation, verticalLayout->GetHeight());
            verticalLayout->MarkContentLayoutDirty();
            break;
        case HorizontalAlignment::Right:
            position.X = GetWidth() - verticalLayout->GetWidth();
            break;
        case HorizontalAlignment::Left:
            break;
        }
        if (!expanded) {
            // TODO: bad code
            position.X = -1000;
            position.Y = -1000;
        }
        verticalLayout->SetPosition(position);
    }

    needToUpdateLayout = false;
}

void TreeView::DoLayout()
{
    UpdateLayout();
}

void TreeView::Draw(DrawingContext& drawingContext)
{
    UpdateLayout();

    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());

    drawingContext.PushTransform(globalPos);

    horizontalLayout->Draw(drawingContext);

    if (expanded) {
        verticalLayout->Draw(drawingContext);
    }

    drawingContext.PopTransform();
}

} // namespace Pomdog::GUI
