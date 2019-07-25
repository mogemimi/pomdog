// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Input/Mouse.hpp"
#include "Pomdog/Input/MouseState.hpp"

namespace Pomdog {

class Event;

} // namespace Pomdog

namespace Pomdog {
namespace Detail {
namespace Cocoa {

class MouseCocoa final : public Mouse {
public:
    MouseCocoa();

    MouseState GetState() const override;

    void HandleEvent(const Event& event);

private:
    MouseState state;
    double scrollWheel;
};

} // namespace Cocoa
} // namespace Detail
} // namespace Pomdog
