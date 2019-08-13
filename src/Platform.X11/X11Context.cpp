// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "X11Context.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog::Detail::X11 {
namespace {

void InitializeAtoms(::Display* display, X11AtomCache & atoms)
{
    atoms.WmDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);
    atoms.Utf8String = XInternAtom(display, "UTF8_STRING", False);

    ([&] {
        auto netSupported = XInternAtom(display, "_NET_SUPPORTED", True);

        if (netSupported == None) {
            return;
        }

        auto netSupportingWmCheck =
            XInternAtom(display, "_NET_SUPPORTING_WM_CHECK", True);

        if (netSupportingWmCheck == None) {
            return;
        }

        atoms.NetWmName = XInternAtom(display, "_NET_WM_NAME", True);
        atoms.NetWmIconName = XInternAtom(display, "_NET_WM_ICON_NAME", True);
    })();
}

} // unnamed namespace

X11Context::X11Context()
    : Display(nullptr)
{
    ::XInitThreads();

    Display = ::XOpenDisplay(nullptr);
    if (Display == nullptr) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Error: XOpenDisplay");
    }

    //int screen = DefaultScreen(display);
    //::Window rootWindow = RootWindow(display, screen);

    InitializeAtoms(Display, Atoms);
}

X11Context::~X11Context()
{
    if (Display != nullptr) {
        ::XCloseDisplay(Display);
        Display = nullptr;
    }
}

} // namespace Pomdog::Detail::X11
