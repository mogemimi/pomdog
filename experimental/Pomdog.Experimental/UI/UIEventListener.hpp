// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog {
namespace UI {

class PointerPoint;

class UIEventListener {
public:
    virtual ~UIEventListener() = default;

    virtual void OnPointerCanceled(PointerPoint const& pointerPoint) = 0;

    virtual void OnPointerCaptureLost(PointerPoint const& pointerPoint) = 0;

    virtual void OnPointerEntered(PointerPoint const& pointerPoint) = 0;

    virtual void OnPointerExited(PointerPoint const& pointerPoint) = 0;

    virtual void OnPointerMoved(PointerPoint const& pointerPoint) = 0;

    virtual void OnPointerPressed(PointerPoint const& pointerPoint) = 0;

    virtual void OnPointerReleased(PointerPoint const& pointerPoint) = 0;

    virtual void OnPointerWheelChanged(PointerPoint const& pointerPoint) = 0;
};

}// namespace UI
}// namespace Pomdog
