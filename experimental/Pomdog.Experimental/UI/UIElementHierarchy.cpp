// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "UIElementHierarchy.hpp"
#include "DrawingContext.hpp"
#include "UIHelper.hpp"

namespace Pomdog {
namespace UI {

UIElementHierarchy::UIElementHierarchy(std::shared_ptr<GameWindow> const& window)
    : dispatcher(std::make_shared<UIEventDispatcher>(window))
{
}

std::shared_ptr<UIEventDispatcher> UIElementHierarchy::Dispatcher() const
{
    POMDOG_ASSERT(dispatcher);
    return dispatcher;
}

void UIElementHierarchy::Touch(MouseState const& mouseState)
{
    POMDOG_ASSERT(dispatcher);
    dispatcher->Touch(mouseState);
    dispatcher->UpdateChildren();
}

void UIElementHierarchy::UpdateLayout()
{
    for (auto & child: children) {
        POMDOG_ASSERT(child);
        child->UpdateTransform();
    }
}

void UIElementHierarchy::AddChild(std::shared_ptr<UIElement> const& element)
{
    POMDOG_ASSERT(element);
    POMDOG_ASSERT(!element->Parent());
    children.push_back(element);
    element->OnEnter();
}

void UIElementHierarchy::AddChild(std::shared_ptr<UIElement> && element)
{
    POMDOG_ASSERT(element);
    POMDOG_ASSERT(!element->Parent());
    children.push_back(element);
    element->OnEnter();
}

void UIElementHierarchy::UpdateAnimation(Duration const& frameDuration)
{
    POMDOG_ASSERT(dispatcher);
    dispatcher->UpdateAnimation(frameDuration);
}

void UIElementHierarchy::Draw(DrawingContext & drawingContext)
{
    UpdateLayout();

    drawingContext.Push(Matrix3x2::Identity);

    for (auto & child: children) {
        POMDOG_ASSERT(child);
        child->Draw(drawingContext);
    }

    drawingContext.Pop();
}

void UIElementHierarchy::RenderSizeChanged(std::uint32_t width, std::uint32_t height)
{
    for (auto & child: children) {
        POMDOG_ASSERT(child);
        if (child->SizeToFitContent()) {
            child->OnRenderSizeChanged(width, height);
        }
    }
}

} // namespace UI
} // namespace Pomdog
