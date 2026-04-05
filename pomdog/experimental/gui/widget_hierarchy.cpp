// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/widget_hierarchy.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/hierarchy_sort_order.h"
#include "pomdog/experimental/gui/ui_helper.h"

namespace pomdog::gui {

WidgetHierarchy::WidgetHierarchy(
    const std::shared_ptr<GameWindow>& window,
    const std::shared_ptr<Keyboard>& keyboard)
    : viewportHeight_(window->getClientBounds().height)
{
    dispatcher_ = std::make_shared<UIEventDispatcher>(window, keyboard);

    dispatcher_->AddContextMenu = [this](const std::shared_ptr<Widget>& widget) {
        this->addChild(widget);
    };

    dispatcher_->RemoveContextMenu = [this](const std::shared_ptr<Widget>& widget) {
        this->removeChild(widget);
    };

    connection_ = window->clientSizeChanged.connect([this](int width, int height) {
        this->renderSizeChanged(width, height);
    });
}

std::shared_ptr<UIEventDispatcher> WidgetHierarchy::getDispatcher() const
{
    POMDOG_ASSERT(dispatcher_);
    return dispatcher_;
}

void WidgetHierarchy::touch(const MouseState& mouseState)
{
    POMDOG_ASSERT(dispatcher_);
    MouseState transposedeState = mouseState;
    transposedeState.position.y = (viewportHeight_ - transposedeState.position.y);
    dispatcher_->touch(transposedeState, children_);
    update();
}

void WidgetHierarchy::addChild(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(widget);
    POMDOG_ASSERT(!widget->getParent());
    subscribeRequests_.push_back(widget);
    widget->onEnter();
}

void WidgetHierarchy::removeChild(const std::weak_ptr<Widget>& child)
{
    if (child.expired()) {
        return;
    }

    auto f = [&](const std::weak_ptr<Widget>& p) { return p.lock() == child.lock(); };

    if (auto iter = std::find_if(std::begin(children_), std::end(children_), f); iter != std::end(children_)) {
        iter->reset();
        POMDOG_ASSERT(*iter == nullptr);
        return;
    }

    auto iter = std::find_if(std::begin(subscribeRequests_), std::end(subscribeRequests_), f);
    if (iter != std::end(subscribeRequests_)) {
        iter->reset();
        POMDOG_ASSERT(*iter == nullptr);
    }
}

bool WidgetHierarchy::contains(const std::shared_ptr<Widget>& child) const
{
    auto f = [&](const std::shared_ptr<Widget>& p) { return p == child; };

    if (auto iter = std::find_if(std::begin(children_), std::end(children_), f); iter != std::end(children_)) {
        return true;
    }

    auto iter = std::find_if(std::begin(subscribeRequests_), std::end(subscribeRequests_), f);
    return iter != std::end(subscribeRequests_);
}

void WidgetHierarchy::update()
{
    if (!subscribeRequests_.empty()) {
        if (children_.empty()) {
            std::swap(children_, subscribeRequests_);
        }
        else {
            children_.reserve(children_.size() + subscribeRequests_.size());
            children_.insert(std::end(children_), std::begin(subscribeRequests_), std::end(subscribeRequests_));
            subscribeRequests_.clear();
        }
        POMDOG_ASSERT(subscribeRequests_.empty());
    }

    auto f = [](const std::shared_ptr<Widget>& p) { return p == nullptr; };
    children_.erase(
        std::remove_if(std::begin(children_), std::end(children_), f),
        std::end(children_));

    if (auto focusedWidget = dispatcher_->getFocusWidget(); focusedWidget != nullptr) {
        auto parent = focusedWidget;
        auto next = focusedWidget;
        while (next != nullptr) {
            parent = next;
            next = parent->getParent();
        }

        if (parent != nullptr) {
            std::stable_sort(std::begin(children_), std::end(children_),
                [&](const std::shared_ptr<Widget>& a, const std::shared_ptr<Widget>& b) -> bool {
                    const auto x = a->getHierarchySortOrder();
                    const auto y = b->getHierarchySortOrder();
                    if ((x == y) && (x == HierarchySortOrder::Sortable)) {
                        return (parent == a);
                    }
                    return x < y;
                });
        }
    }

    POMDOG_ASSERT(std::end(children_) == std::unique(std::begin(children_), std::end(children_),
                                             [](const std::shared_ptr<Widget>& a, const std::shared_ptr<Widget>& b) { return a == b; }));
}

void WidgetHierarchy::updateAnimation(const Duration& frameDuration)
{
    POMDOG_ASSERT(dispatcher_);
    dispatcher_->updateAnimation(frameDuration);

    for (const auto& child : children_) {
        if (child != nullptr) {
            child->updateAnimation(frameDuration);
        }
    }
}

void WidgetHierarchy::draw(DrawingContext& drawingContext)
{
    drawingContext.pushTransform(Point2D{0, 0});

    std::for_each(std::rbegin(children_), std::rend(children_), [&](const std::shared_ptr<Widget>& child) {
        POMDOG_ASSERT(child != nullptr);
        child->draw(drawingContext);
    });

    drawingContext.popTransform();
}

void WidgetHierarchy::renderSizeChanged(int width, int height)
{
    const auto translationOffset = Point2D{0, height - viewportHeight_};

    viewportHeight_ = height;

    for (const auto& child : children_) {
        POMDOG_ASSERT(child != nullptr);

        if (child->getSizeToFitContent()) {
            child->setSize(width, height);
            child->markContentLayoutDirty();
        }
        else {
            const auto position = child->getPosition();
            child->setPosition(position + translationOffset);
        }
    }
}

} // namespace pomdog::gui
