// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/widget_hierarchy.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/hierarchy_sort_order.h"
#include "pomdog/experimental/gui/ui_helper.h"

namespace pomdog::gui {

WidgetHierarchy::WidgetHierarchy(
    const std::shared_ptr<GameWindow>& window,
    const std::shared_ptr<Keyboard>& keyboard)
    : viewportHeight(window->GetClientBounds().Height)
{
    dispatcher = std::make_shared<UIEventDispatcher>(window, keyboard);

    dispatcher->AddContextMenu = [this](const std::shared_ptr<Widget>& widget) {
        this->AddChild(widget);
    };

    dispatcher->RemoveContextMenu = [this](const std::shared_ptr<Widget>& widget) {
        this->RemoveChild(widget);
    };

    connection = window->ClientSizeChanged.Connect([this](int width, int height) {
        this->RenderSizeChanged(width, height);
    });
}

std::shared_ptr<UIEventDispatcher> WidgetHierarchy::GetDispatcher() const
{
    POMDOG_ASSERT(dispatcher);
    return dispatcher;
}

void WidgetHierarchy::Touch(const MouseState& mouseState)
{
    POMDOG_ASSERT(dispatcher);
    MouseState transposedeState = mouseState;
    transposedeState.Position.Y = (viewportHeight - transposedeState.Position.Y);
    dispatcher->Touch(transposedeState, children);
    Update();
}

void WidgetHierarchy::AddChild(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(widget);
    POMDOG_ASSERT(!widget->GetParent());
    subscribeRequests.push_back(widget);
    widget->OnEnter();
}

void WidgetHierarchy::RemoveChild(const std::weak_ptr<Widget>& child)
{
    if (child.expired()) {
        return;
    }

    auto f = [&](const std::weak_ptr<Widget>& p) { return p.lock() == child.lock(); };

    if (auto iter = std::find_if(std::begin(children), std::end(children), f); iter != std::end(children)) {
        iter->reset();
        POMDOG_ASSERT(*iter == nullptr);
        return;
    }

    auto iter = std::find_if(std::begin(subscribeRequests), std::end(subscribeRequests), f);
    if (iter != std::end(subscribeRequests)) {
        iter->reset();
        POMDOG_ASSERT(*iter == nullptr);
    }
}

bool WidgetHierarchy::Contains(const std::shared_ptr<Widget>& child) const
{
    auto f = [&](const std::shared_ptr<Widget>& p) { return p == child; };

    if (auto iter = std::find_if(std::begin(children), std::end(children), f); iter != std::end(children)) {
        return true;
    }

    auto iter = std::find_if(std::begin(subscribeRequests), std::end(subscribeRequests), f);
    return iter != std::end(subscribeRequests);
}

void WidgetHierarchy::Update()
{
    if (!subscribeRequests.empty()) {
        if (children.empty()) {
            std::swap(children, subscribeRequests);
        }
        else {
            children.reserve(children.size() + subscribeRequests.size());
            children.insert(std::end(children), std::begin(subscribeRequests), std::end(subscribeRequests));
            subscribeRequests.clear();
        }
        POMDOG_ASSERT(subscribeRequests.empty());
    }

    auto f = [](const std::shared_ptr<Widget>& p) { return p == nullptr; };
    children.erase(
        std::remove_if(std::begin(children), std::end(children), f),
        std::end(children));

    if (auto focusedWidget = dispatcher->GetFocusWidget(); focusedWidget != nullptr) {
        auto parent = focusedWidget;
        auto next = focusedWidget;
        while (next != nullptr) {
            parent = next;
            next = parent->GetParent();
        }

        if (parent != nullptr) {
            std::stable_sort(std::begin(children), std::end(children),
                [&](const std::shared_ptr<Widget>& a, const std::shared_ptr<Widget>& b) -> bool {
                    const auto x = a->GetHierarchySortOrder();
                    const auto y = b->GetHierarchySortOrder();
                    if ((x == y) && (x == HierarchySortOrder::Sortable)) {
                        return (parent == a);
                    }
                    return x < y;
                });
        }
    }

    POMDOG_ASSERT(std::end(children) == std::unique(std::begin(children), std::end(children),
        [](const std::shared_ptr<Widget>& a, const std::shared_ptr<Widget>& b) { return a == b; }));
}

void WidgetHierarchy::UpdateAnimation(const Duration& frameDuration)
{
    POMDOG_ASSERT(dispatcher);
    dispatcher->UpdateAnimation(frameDuration);

    for (const auto& child : children) {
        if (child != nullptr) {
            child->UpdateAnimation(frameDuration);
        }
    }
}

void WidgetHierarchy::Draw(DrawingContext& drawingContext)
{
    drawingContext.PushTransform(Point2D{0, 0});

    std::for_each(std::rbegin(children), std::rend(children), [&](const std::shared_ptr<Widget>& child) {
        POMDOG_ASSERT(child != nullptr);
        child->Draw(drawingContext);
    });

    drawingContext.PopTransform();
}

void WidgetHierarchy::RenderSizeChanged(int width, int height)
{
    const auto translationOffset = Point2D{0, height - viewportHeight};

    viewportHeight = height;

    for (const auto& child : children) {
        POMDOG_ASSERT(child != nullptr);

        if (child->GetSizeToFitContent()) {
            child->SetSize(width, height);
            child->MarkContentLayoutDirty();
        }
        else {
            const auto position = child->GetPosition();
            child->SetPosition(position + translationOffset);
        }
    }
}

} // namespace pomdog::gui
