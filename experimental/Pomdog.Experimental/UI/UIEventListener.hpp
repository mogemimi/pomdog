// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_UIEVENTLISTENER_6C74CE34_HPP
#define POMDOG_UIEVENTLISTENER_6C74CE34_HPP

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

#endif // POMDOG_UIEVENTLISTENER_6C74CE34_HPP
