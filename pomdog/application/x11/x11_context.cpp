// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/x11/x11_context.h"

namespace pomdog::detail::x11 {

X11Context::X11Context() noexcept = default;

std::unique_ptr<Error>
X11Context::initialize() noexcept
{
    ::XInitThreads();

    auto& atoms = this->Atoms;
    auto& display = this->Display;

    // NOTE: Connect to the X server.
    display = ::XOpenDisplay(nullptr);
    if (display == nullptr) {
        return errors::make("XOpenDisplay() failed");
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
        atoms.NetWmState = XInternAtom(display, "_NET_WM_STATE", True);
        atoms.NetWmStateFullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", True);
        atoms.NetWmStateMaximizedVert = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", True);
        atoms.NetWmStateMaximizedHorz = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", True);
        atoms.MotifWmHints = XInternAtom(display, "_MOTIF_WM_HINTS", False);
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
