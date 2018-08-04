// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Input/Mouse.hpp"
#include "Pomdog/Input/MouseState.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"

namespace Pomdog {
namespace Detail {
namespace Win32 {

class MouseWin32 final : public Mouse {
public:
    explicit MouseWin32(HWND windowHandle);

    MouseState GetState() const override;

    void HandleMessage(const RAWMOUSE& mouse);

private:
    HWND windowHandle;
    MouseState previousState;
    MouseState state;
};

} // namespace Win32
} // namespace Detail
} // namespace Pomdog
