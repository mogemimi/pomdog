// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/widget_hierarchy.h"
#include "pomdog/application/display_metrics.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/hierarchy_sort_order.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/input/gesture_tracker.h"
#include "pomdog/input/mouse.h"
#include "pomdog/input/mouse_buttons.h"
#include "pomdog/input/touchscreen.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {
namespace {

[[nodiscard]] i32
toGUIUnits(i32 logicalValue, f32 scale) noexcept
{
    const f32 invScale = (scale > 0.0f) ? (1.0f / scale) : 1.0f;
    return static_cast<i32>(std::lround(static_cast<f32>(logicalValue) * invScale));
}

} // namespace

WidgetHierarchy::WidgetHierarchy(
    const std::shared_ptr<GameWindow>& window,
    const std::shared_ptr<Keyboard>& keyboard)
    : window_(window)
    , viewportHeight_(window->getClientBounds().height)
{
    dispatcher_ = std::make_shared<UIEventDispatcher>(window, keyboard);

    addContextMenuConnection_ = dispatcher_->AddContextMenu.connect([this](const std::shared_ptr<Widget>& widget) {
        this->addChild(widget);
    });

    removeContextMenuConnection_ = dispatcher_->RemoveContextMenu.connect([this](const std::shared_ptr<Widget>& widget) {
        this->removeChild(widget);
    });

    clientSizeChangedConnection_ = window->displayMetricsChanged.connect([this](const DisplayMetrics& m) {
        this->renderSizeChanged(
            toGUIUnits(m.clientBounds.width, uiScale_),
            toGUIUnits(m.clientBounds.height, uiScale_));
    });
}

std::shared_ptr<UIEventDispatcher> WidgetHierarchy::getDispatcher() const
{
    POMDOG_ASSERT(dispatcher_);
    return dispatcher_;
}

void WidgetHierarchy::touch(const Mouse& mouse, raw_ptr<const Touchscreen> touchscreen)
{
    POMDOG_ASSERT(dispatcher_);

    // NOTE: Combine the mouse and the touchscreen into a single pointer. A
    // pressed touch is treated as the primary (left) button.
    const auto gesture = getPrimaryGestureState(mouse, touchscreen);

    // NOTE: Convert the logical pointer position into GUI space (dividing by
    // the UI scale), then flip Y so the origin matches the bottom-left widget
    // layout.
    auto position = Point2D{
        toGUIUnits(gesture.position.x, uiScale_),
        toGUIUnits(gesture.position.y, uiScale_),
    };
    position.y = (viewportHeight_ - position.y);

    PointerEventInput input;
    input.leftDown = mouse.isButtonDown(MouseButtons::Left) ||
                     ((touchscreen != nullptr) && touchscreen->isPrimaryTouchPressed());
    input.rightDown = mouse.isButtonDown(MouseButtons::Right);
    input.middleDown = mouse.isButtonDown(MouseButtons::Middle);
    input.x1Down = mouse.isButtonDown(MouseButtons::X1);
    input.x2Down = mouse.isButtonDown(MouseButtons::X2);
    input.scrollY = mouse.getScrollY();

    dispatcher_->touch(position, input, children_);
    dispatcher_->dispatchTextInputFromKeyboard();
    update();
}

void WidgetHierarchy::setUIScale(f32 uiScale)
{
    uiScale_ = (uiScale > 0.0f) ? uiScale : 1.0f;

    // NOTE: Re-layout immediately so the GUI-space size reflects the new UI
    // scale even before the next displayMetricsChanged event.
    if (window_ != nullptr) {
        const auto bounds = window_->getClientBounds();
        renderSizeChanged(
            toGUIUnits(bounds.width, uiScale_),
            toGUIUnits(bounds.height, uiScale_));
    }
}

f32 WidgetHierarchy::getUIScale() const noexcept
{
    return uiScale_;
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

void WidgetHierarchy::renderSizeChanged(i32 width, i32 height)
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
