// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/x11/x11_context.hpp"

namespace pomdog::detail::x11 {

X11Context::X11Context() noexcept = default;

std::unique_ptr<Error>
X11Context::Initialize() noexcept
{
    ::XInitThreads();

    auto& atoms = this->Atoms;
    auto& display = this->Display;

    // NOTE: Connect to the X server.
    display = ::XOpenDisplay(nullptr);
    if (display == nullptr) {
        return errors::New("XOpenDisplay() failed");
    }

    // NOTE: Initialize Atoms.
    atoms.WmDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);
    atoms.Utf8String = XInternAtom(display, "UTF8_STRING", False);

    [&] {
        auto netSupported = XInternAtom(display, "_NET_SUPPORTED", True);
        if (netSupported == None) {
            return;
        }

        auto netSupportingWmCheck = XInternAtom(display, "_NET_SUPPORTING_WM_CHECK", True);
        if (netSupportingWmCheck == None) {
            return;
        }

        atoms.NetWmName = XInternAtom(display, "_NET_WM_NAME", True);
        atoms.NetWmIconName = XInternAtom(display, "_NET_WM_ICON_NAME", True);
    }();

    return nullptr;
}

X11Context::~X11Context() noexcept
{
    if (Display != nullptr) {
        ::XCloseDisplay(Display);
        Display = nullptr;
    }
}

} // namespace pomdog::detail::x11
