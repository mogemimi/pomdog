// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/UI/UIElement.hpp"
#include "Pomdog.Experimental/UI/detail/UIEventConnection.hpp"
#include "Pomdog.Experimental/Gameplay/Entity.hpp"
#include <Pomdog/Pomdog.hpp>
#include <optional>

namespace Pomdog {
namespace UI {

class ScenePanel final
    : public UIElement
    , public std::enable_shared_from_this<ScenePanel> {
public:
    ScenePanel(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        int width,
        int height);

    bool IsEnabled() const;
    void SetEnabled(bool isEnabled);

    bool IsFocused() const;

    double GetScrollWheel() const;

    bool SizeToFitContent() const override { return true; }

    void OnEnter() override;

    void OnPointerWheelChanged(const PointerPoint& pointerPoint) override;

    void OnPointerEntered(const PointerPoint& pointerPoint) override;

    void OnPointerExited(const PointerPoint& pointerPoint) override;

    void OnPointerPressed(const PointerPoint& pointerPoint) override;

    void OnPointerMoved(const PointerPoint& pointerPoint) override;

    void OnPointerReleased(const PointerPoint& pointerPoint) override;

    void Draw(DrawingContext & drawingContext) override;

    void UpdateAnimation(const Duration& frameDuration) override;

    // Events:
    Signal<void(const Vector2& point)> SceneTouch;

private:
    void OnMouseLeftButtonPressed(const PointerPoint& pointerPoint);
    void OnMouseLeftButtonMoved(const PointerPoint& pointerPoint);
    void OnMouseMiddleButtonPressed(const PointerPoint& pointerPoint);
    void OnMouseMiddleButtonMoved(const PointerPoint& pointerPoint);
    void OnMouseRightButtonPressed(const PointerPoint& pointerPoint);
    void OnMouseRightButtonMoved(const PointerPoint& pointerPoint);

    Vector2 ConvertToPanelSpace(const Point2D& point) const;

private:
    Detail::UIEventConnection connection;

    std::optional<Vector2> tumbleStartPosition;
    std::optional<Vector2> trackStartPosition;

    Duration timer;
    float normalizedScrollDirection;
    float scrollAcceleration;
    double cameraZoom;

    bool isFocused;
    bool isEnabled;

    class ScrollWheelSampler {
    private:
        ///@todo replace with std::deque<float>
        std::optional<float> average;

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
