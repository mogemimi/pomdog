// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/GUI/ScenePanel.hpp"
#include "Pomdog/Experimental/GUI/PointerPoint.hpp"
#include "Pomdog/Experimental/GUI/UIEventDispatcher.hpp"
#include <cmath>

namespace Pomdog::GUI {

ScenePanelController::~ScenePanelController() = default;

ScenePanel::ScenePanel(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    int widthIn,
    int heightIn)
    : Widget(dispatcher)
    , isFocused(false)
    , isEnabled(true)
{
    SetSize(widthIn, heightIn);
    SetHierarchySortOrder(HierarchySortOrder::Back);
}

bool ScenePanel::IsEnabled() const noexcept
{
    return isEnabled;
}

void ScenePanel::SetEnabled(bool isEnabledIn) noexcept
{
    this->isEnabled = isEnabledIn;
}

bool ScenePanel::IsFocused() const noexcept
{
    return this->isFocused;
}

bool ScenePanel::GetSizeToFitContent() const noexcept
{
    return true;
}

Point2D ScenePanel::ConvertToPanelSpace(const Point2D& point) const noexcept
{
    return Point2D(point.X - GetX(), point.Y - GetY());
}

void ScenePanel::OnEnter()
{
    isFocused = false;
}

void ScenePanel::SetController(std::unique_ptr<ScenePanelController>&& controllerIn) noexcept
{
    controller = std::move(controllerIn);
}

ScenePanelController* ScenePanel::GetController() const noexcept
{
    return controller.get();
}

void ScenePanel::OnKeyDown(const KeyboardState& keyboardState, Keys key)
{
    if (!isEnabled) {
        return;
    }
    if (controller != nullptr) {
        controller->OnKeyDown(this, keyboardState, key);
    }
}

void ScenePanel::OnKeyUp(const KeyboardState& keyboardState, Keys key)
{
    if (!isEnabled) {
        return;
    }
    if (controller != nullptr) {
        controller->OnKeyUp(this, keyboardState, key);
    }
}

void ScenePanel::OnPointerWheelChanged(const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }
    if (controller != nullptr) {
        controller->OnPointerWheelChanged(this, pointerPoint);
    }
}

void ScenePanel::OnPointerEntered(const PointerPoint& pointerPoint)
{
    isFocused = true;

    if (!isEnabled) {
        return;
    }
    if (controller != nullptr) {
        controller->OnPointerEntered(this, pointerPoint);
    }
}

void ScenePanel::OnPointerExited(const PointerPoint& pointerPoint)
{
    isFocused = false;

    if (!isEnabled) {
        return;
    }
    if (controller != nullptr) {
        controller->OnPointerExited(this, pointerPoint);
    }
}

void ScenePanel::OnPointerPressed(const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }
    if (controller != nullptr) {
        controller->OnPointerPressed(this, pointerPoint);
    }
}

void ScenePanel::OnPointerMoved(const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }
    if (controller != nullptr) {
        controller->OnPointerMoved(this, pointerPoint);
    }
}

void ScenePanel::OnPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }
    if (controller != nullptr) {
        controller->OnPointerReleased(this, pointerPoint);
    }
}

} // namespace Pomdog::GUI
