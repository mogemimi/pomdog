// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/scene_panel.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {

ScenePanelController::~ScenePanelController() = default;

ScenePanel::ScenePanel(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    int widthIn,
    int heightIn)
    : Widget(dispatcher)
    , isFocused_(false)
    , isEnabled_(true)
{
    setSize(widthIn, heightIn);
    setHierarchySortOrder(HierarchySortOrder::Back);
}

bool ScenePanel::isEnabled() const noexcept
{
    return isEnabled_;
}

void ScenePanel::setEnabled(bool isEnabledIn) noexcept
{
    isEnabled_ = isEnabledIn;
}

bool ScenePanel::isFocused() const noexcept
{
    return isFocused_;
}

bool ScenePanel::getSizeToFitContent() const noexcept
{
    return true;
}

Point2D ScenePanel::convertToPanelSpace(const Point2D& point) const noexcept
{
    return Point2D(point.x - getX(), point.y - getY());
}

void ScenePanel::onEnter()
{
    isFocused_ = false;
}

void ScenePanel::setController(std::unique_ptr<ScenePanelController>&& controllerIn) noexcept
{
    controller_ = std::move(controllerIn);
}

ScenePanelController* ScenePanel::getController() const noexcept
{
    return controller_.get();
}

void ScenePanel::onKeyDown(const KeyboardState& keyboardState, Keys key)
{
    if (!isEnabled_) {
        return;
    }
    if (controller_ != nullptr) {
        controller_->onKeyDown(this, keyboardState, key);
    }
}

void ScenePanel::onKeyUp(const KeyboardState& keyboardState, Keys key)
{
    if (!isEnabled_) {
        return;
    }
    if (controller_ != nullptr) {
        controller_->onKeyUp(this, keyboardState, key);
    }
}

void ScenePanel::onPointerWheelChanged(const PointerPoint& pointerPoint)
{
    if (!isEnabled_) {
        return;
    }
    if (controller_ != nullptr) {
        controller_->onPointerWheelChanged(this, pointerPoint);
    }
}

void ScenePanel::onPointerEntered(const PointerPoint& pointerPoint)
{
    isFocused_ = true;

    if (!isEnabled_) {
        return;
    }
    if (controller_ != nullptr) {
        controller_->onPointerEntered(this, pointerPoint);
    }
}

void ScenePanel::onPointerExited(const PointerPoint& pointerPoint)
{
    isFocused_ = false;

    if (!isEnabled_) {
        return;
    }
    if (controller_ != nullptr) {
        controller_->onPointerExited(this, pointerPoint);
    }
}

void ScenePanel::onPointerPressed(const PointerPoint& pointerPoint)
{
    if (!isEnabled_) {
        return;
    }
    if (controller_ != nullptr) {
        controller_->onPointerPressed(this, pointerPoint);
    }
}

void ScenePanel::onPointerMoved(const PointerPoint& pointerPoint)
{
    if (!isEnabled_) {
        return;
    }
    if (controller_ != nullptr) {
        controller_->onPointerMoved(this, pointerPoint);
    }
}

void ScenePanel::onPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled_) {
        return;
    }
    if (controller_ != nullptr) {
        controller_->onPointerReleased(this, pointerPoint);
    }
}

} // namespace pomdog::gui
