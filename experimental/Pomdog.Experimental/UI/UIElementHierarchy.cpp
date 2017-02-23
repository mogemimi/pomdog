// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/UI/UIElementHierarchy.hpp"
#include "Pomdog.Experimental/UI/DrawingContext.hpp"
#include "Pomdog.Experimental/UI/UIHelper.hpp"

namespace Pomdog {
namespace UI {

UIElementHierarchy::UIElementHierarchy(const std::shared_ptr<GameWindow>& window)
    : dispatcher(std::make_shared<UIEventDispatcher>(window))
    , viewportHeight(window->GetClientBounds().Height)
{
}

std::shared_ptr<UIEventDispatcher> UIElementHierarchy::GetDispatcher() const
{
    POMDOG_ASSERT(dispatcher);
    return dispatcher;
}

void UIElementHierarchy::Touch(const MouseState& mouseState)
{
    POMDOG_ASSERT(dispatcher);
    MouseState transposedeState = mouseState;
    transposedeState.Position.Y = (viewportHeight - transposedeState.Position.Y);
    dispatcher->Touch(transposedeState);
    dispatcher->UpdateChildren();
}

void UIElementHierarchy::AddChild(const std::shared_ptr<UIElement>& element)
{
    POMDOG_ASSERT(element);
    POMDOG_ASSERT(!element->GetParent());
    children.push_back(element);
    element->OnEnter();
}

void UIElementHierarchy::UpdateAnimation(const Duration& frameDuration)
{
    POMDOG_ASSERT(dispatcher);
    dispatcher->UpdateAnimation(frameDuration);
}

void UIElementHierarchy::Draw(DrawingContext & drawingContext)
{
    drawingContext.Push(Matrix3x2::Identity);

    for (auto & child : children) {
        POMDOG_ASSERT(child);
        child->Draw(drawingContext);
    }

    drawingContext.Pop();
}

void UIElementHierarchy::RenderSizeChanged(int width, int height)
{
    viewportHeight = height;
    for (auto & child : children) {
        POMDOG_ASSERT(child);
        if (child->SizeToFitContent()) {
            child->SetSize(width, height);
            child->MarkContentLayoutDirty();
        }
    }
}

} // namespace UI
} // namespace Pomdog
