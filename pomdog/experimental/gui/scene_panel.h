// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/widget.h"
#include "pomdog/math/point2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {
class ScenePanel;
} // namespace pomdog::gui

namespace pomdog::gui {

class ScenePanelController {
public:
    virtual ~ScenePanelController();

    virtual void
    onKeyDown(ScenePanel* scenePanel, const KeyboardState& keyboardState, Keys key) = 0;

    virtual void
    onKeyUp(ScenePanel* scenePanel, const KeyboardState& keyboardState, Keys key) = 0;

    virtual void
    onPointerWheelChanged(ScenePanel* scenePanel, const PointerPoint& pointerPoint) = 0;

    virtual void
    onPointerEntered(ScenePanel* scenePanel, const PointerPoint& pointerPoint) = 0;

    virtual void
    onPointerExited(ScenePanel* scenePanel, const PointerPoint& pointerPoint) = 0;

    virtual void
    onPointerPressed(ScenePanel* scenePanel, const PointerPoint& pointerPoint) = 0;

    virtual void
    onPointerMoved(ScenePanel* scenePanel, const PointerPoint& pointerPoint) = 0;

    virtual void
    onPointerReleased(ScenePanel* scenePanel, const PointerPoint& pointerPoint) = 0;
};

class ScenePanel final
    : public Widget
    , public std::enable_shared_from_this<ScenePanel> {
public:
    ScenePanel(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        int width,
        int height);

    [[nodiscard]] bool
    isEnabled() const noexcept;

    void
    setEnabled(bool isEnabled) noexcept;

    [[nodiscard]] bool
    isFocused() const noexcept;

    [[nodiscard]] bool
    getSizeToFitContent() const noexcept override;

    void
    onEnter() override;

    void
    setController(std::unique_ptr<ScenePanelController>&& controller) noexcept;

    [[nodiscard]] ScenePanelController* getController() const noexcept;

    void
    onKeyDown(const KeyboardState& keyboardState, Keys key) override;

    void
    onKeyUp(const KeyboardState& keyboardState, Keys key) override;

    void
    onPointerWheelChanged(const PointerPoint& pointerPoint) override;

    void
    onPointerEntered(const PointerPoint& pointerPoint) override;

    void
    onPointerExited(const PointerPoint& pointerPoint) override;

    void
    onPointerPressed(const PointerPoint& pointerPoint) override;

    void
    onPointerMoved(const PointerPoint& pointerPoint) override;

    void
    onPointerReleased(const PointerPoint& pointerPoint) override;

    [[nodiscard]] Point2D
    convertToPanelSpace(const Point2D& point) const noexcept;

private:
    std::unique_ptr<ScenePanelController> controller_;
    bool isFocused_ = false;
    bool isEnabled_ = true;
};

} // namespace pomdog::gui
