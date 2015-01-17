// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "UIElement.hpp"
#include "detail/UIEventConnection.hpp"
#include "Pomdog.Experimental/Gameplay/GameObject.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {
namespace UI {

class ScenePanel: public UIElement, public std::enable_shared_from_this<ScenePanel> {
public:
    ScenePanel(std::shared_ptr<UIEventDispatcher> const& dispatcher,
        int widthIn, int heightIn);

    bool IsEnabled() const;
    void IsEnabled(bool isEnabled);

    bool IsFocused() const;

    double GetScrollWheel() const;

    bool SizeToFitContent() const override { return true; }

    void OnEnter() override;

    void OnPointerWheelChanged(PointerPoint const& pointerPoint) override;

    void OnPointerEntered(PointerPoint const& pointerPoint) override;

    void OnPointerExited(PointerPoint const& pointerPoint) override;

    void OnPointerPressed(PointerPoint const& pointerPoint) override;

    void OnPointerMoved(PointerPoint const& pointerPoint) override;

    void OnPointerReleased(PointerPoint const& pointerPoint) override;

    void OnRenderSizeChanged(int width, int height) override;

    void Draw(DrawingContext & drawingContext) override;

    void UpdateAnimation(Duration const& frameDuration) override;

    // Events:
    Signal<void(Vector2 const& point)> SceneTouch;

private:
    void OnMouseLeftButtonPressed(PointerPoint const& pointerPoint);
    void OnMouseLeftButtonMoved(PointerPoint const& pointerPoint);
    void OnMouseMiddleButtonPressed(PointerPoint const& pointerPoint);
    void OnMouseMiddleButtonMoved(PointerPoint const& pointerPoint);
    void OnMouseRightButtonPressed(PointerPoint const& pointerPoint);
    void OnMouseRightButtonMoved(PointerPoint const& pointerPoint);

    Vector2 ConvertToPanelSpace(Point2D const& point) const;

private:
    Detail::UIEventConnection connection;

    Optional<Vector2> tumbleStartPosition;
    Optional<Vector2> trackStartPosition;

    Duration timer;
    float normalizedScrollDirection;
    float scrollAcceleration;
    double cameraZoom;

    bool isFocused;
    bool isEnabled;

    class ScrollWheelSampler {
    private:
        ///@todo replace with std::deque<float>
        Optional<float> average;

    public:
        void AddWheelDelta(int wheelDelta)
        {
            if (wheelDelta == 0) {
                return;
            }

            if (!average) {
                average = std::abs(wheelDelta);
            }
            else {
                average = std::max((*average + std::abs(wheelDelta)) / 2, 1.0f);
            }
        }

        float GetScrollWheelDeltaAverage() const
        {
            if (average) {
                return *average;
            }
            return 1.0f;
        }
    };

    ScrollWheelSampler scrollWheelSampler;
};

} // namespace UI
} // namespace Pomdog
