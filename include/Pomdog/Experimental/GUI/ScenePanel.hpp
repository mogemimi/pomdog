// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/GUI/Widget.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Signals/Signal.hpp"
#include <memory>
#include <optional>

namespace Pomdog::GUI {

class ScenePanel final
    : public Widget
    , public std::enable_shared_from_this<ScenePanel> {
public:
    ScenePanel(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        int width,
        int height);

    bool IsEnabled() const;
    void SetEnabled(bool isEnabled);

    bool IsFocused() const;

    bool GetSizeToFitContent() const noexcept override;

    void OnEnter() override;

    void OnPointerWheelChanged(const PointerPoint& pointerPoint) override;

    void OnPointerEntered(const PointerPoint& pointerPoint) override;

    void OnPointerExited(const PointerPoint& pointerPoint) override;

    void OnPointerPressed(const PointerPoint& pointerPoint) override;

    void OnPointerMoved(const PointerPoint& pointerPoint) override;

    void OnPointerReleased(const PointerPoint& pointerPoint) override;

    void Draw(DrawingContext& drawingContext) override;

    void UpdateAnimation(const Duration& frameDuration) override;

    Signal<void(const Vector2& point)> SceneTouch;

    Signal<void(double)> ScrollWheelChanged;

private:
    void OnMouseLeftButtonPressed(const PointerPoint& pointerPoint);
    void OnMouseLeftButtonMoved(const PointerPoint& pointerPoint);
    void OnMouseMiddleButtonPressed(const PointerPoint& pointerPoint);
    void OnMouseMiddleButtonMoved(const PointerPoint& pointerPoint);
    void OnMouseRightButtonPressed(const PointerPoint& pointerPoint);
    void OnMouseRightButtonMoved(const PointerPoint& pointerPoint);

    Vector2 ConvertToPanelSpace(const Point2D& point) const;

private:
    ScopedConnection connection;

    std::optional<Vector2> tumbleStartPosition;
    std::optional<Vector2> trackStartPosition;

    Duration timer;
    float normalizedScrollDirection;
    float scrollAcceleration;
    double cameraZoom;

    bool isFocused;
    bool isEnabled;

    class ScrollWheelSampler final {
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
                average = static_cast<float>(std::abs(wheelDelta));
            }
            else {
                average = std::max((*average + static_cast<float>(std::abs(wheelDelta))) / 2, 1.0f);
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

} // namespace Pomdog::GUI
