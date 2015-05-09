// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_MOUSEWIN32_0AE8AABE_HPP
#define POMDOG_MOUSEWIN32_0AE8AABE_HPP

#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include "Pomdog/Input/Mouse.hpp"
#include "Pomdog/Input/MouseState.hpp"

namespace Pomdog {
namespace Detail {
namespace Win32 {

class MouseWin32 final : public Mouse {
public:
    explicit MouseWin32(HWND windowHandle);

    MouseState GetState() const override;

    void HandleMessage(RAWMOUSE const& mouse);

private:
    HWND windowHandle;
    MouseState previousState;
    MouseState state;
};

} // namespace Win32
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_MOUSEWIN32_0AE8AABE_HPP
