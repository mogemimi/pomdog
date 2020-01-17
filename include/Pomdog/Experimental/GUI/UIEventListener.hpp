// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog {
namespace UI {

class PointerPoint;

class UIEventListener {
public:
    virtual ~UIEventListener() = default;

    virtual void OnPointerCanceled(const PointerPoint& pointerPoint) = 0;

    virtual void OnPointerCaptureLost(const PointerPoint& pointerPoint) = 0;

    virtual void OnPointerEntered(const PointerPoint& pointerPoint) = 0;

    virtual void OnPointerExited(const PointerPoint& pointerPoint) = 0;

    virtual void OnPointerMoved(const PointerPoint& pointerPoint) = 0;

    virtual void OnPointerPressed(const PointerPoint& pointerPoint) = 0;

    virtual void OnPointerReleased(const PointerPoint& pointerPoint) = 0;

    virtual void OnPointerWheelChanged(const PointerPoint& pointerPoint) = 0;
};

} // namespace UI
} // namespace Pomdog
