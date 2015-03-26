// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_MOUSECOCOA_E737D799_HPP
#define POMDOG_MOUSECOCOA_E737D799_HPP

#include "Pomdog/Input/Mouse.hpp"
#include "Pomdog/Input/MouseState.hpp"

namespace Pomdog {
namespace Detail {
namespace Cocoa {

class MouseCocoa final: public Mouse {
public:
    MouseCocoa();

    ///@copydoc Pomdog::Mouse
    MouseState GetState() const override;

    void Position(Point2D const& position);
    void LeftButton(ButtonState buttonState);
    void RightButton(ButtonState buttonState);
    void MiddleButton(ButtonState buttonState);
    void XButton1(ButtonState buttonState);
    void XButton2(ButtonState buttonState);

    void WheelDelta(double wheelDelta);

private:
    MouseState state;
    double scrollWheel;
};

}// namespace Cocoa
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_MOUSECOCOA_E737D799_HPP
