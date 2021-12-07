// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/gui/widget.hpp"
#include "pomdog/math/point2d.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::GUI {

class ScenePanel;

class ScenePanelController {
public:
    virtual ~ScenePanelController();

    virtual void OnKeyDown(ScenePanel* scenePanel, const KeyboardState& keyboardState, Keys key) = 0;

    virtual void OnKeyUp(ScenePanel* scenePanel, const KeyboardState& keyboardState, Keys key) = 0;

    virtual void OnPointerWheelChanged(ScenePanel* scenePanel, const PointerPoint& pointerPoint) = 0;

    virtual void OnPointerEntered(ScenePanel* scenePanel, const PointerPoint& pointerPoint) = 0;

    virtual void OnPointerExited(ScenePanel* scenePanel, const PointerPoint& pointerPoint) = 0;

    virtual void OnPointerPressed(ScenePanel* scenePanel, const PointerPoint& pointerPoint) = 0;

    virtual void OnPointerMoved(ScenePanel* scenePanel, const PointerPoint& pointerPoint) = 0;

    virtual void OnPointerReleased(ScenePanel* scenePanel, const PointerPoint& pointerPoint) = 0;
};

class ScenePanel final
    : public Widget
    , public std::enable_shared_from_this<ScenePanel> {
public:
    ScenePanel(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        int width,
        int height);

    bool IsEnabled() const noexcept;
    void SetEnabled(bool isEnabled) noexcept;

    bool IsFocused() const noexcept;

    bool GetSizeToFitContent() const noexcept override;

    void OnEnter() override;

    void SetController(std::unique_ptr<ScenePanelController>&& controller) noexcept;

    [[nodiscard]] ScenePanelController* GetController() const noexcept;

    void OnKeyDown(const KeyboardState& keyboardState, Keys key) override;

    void OnKeyUp(const KeyboardState& keyboardState, Keys key) override;

    void OnPointerWheelChanged(const PointerPoint& pointerPoint) override;

    void OnPointerEntered(const PointerPoint& pointerPoint) override;

    void OnPointerExited(const PointerPoint& pointerPoint) override;

    void OnPointerPressed(const PointerPoint& pointerPoint) override;

    void OnPointerMoved(const PointerPoint& pointerPoint) override;

    void OnPointerReleased(const PointerPoint& pointerPoint) override;

    Point2D ConvertToPanelSpace(const Point2D& point) const noexcept;

private:
    std::unique_ptr<ScenePanelController> controller;
    bool isFocused = false;
    bool isEnabled = true;
};

} // namespace Pomdog::GUI
