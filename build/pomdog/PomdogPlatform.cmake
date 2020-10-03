target_sources(pomdog_static PRIVATE
    $<$<PLATFORM_ID:Darwin,iOS>:
        # NOTE: Platform.Apple
        ${POMDOG_SRC_DIR}/Platform.Apple/FileSystemApple.mm
        ${POMDOG_SRC_DIR}/Platform.Apple/TimeSourceApple.cpp
        ${POMDOG_SRC_DIR}/Platform.Apple/TimeSourceApple.hpp
    >

    $<$<PLATFORM_ID:Darwin>:
        # NOTE: Platform.Cocoa
        ${POMDOG_INC_DIR}/Platform/Cocoa/Bootstrap.hpp
        ${POMDOG_SRC_DIR}/Platform.Cocoa/Bootstrap.mm
        ${POMDOG_SRC_DIR}/Platform.Cocoa/CocoaWindowDelegate.hpp
        ${POMDOG_SRC_DIR}/Platform.Cocoa/CocoaWindowDelegate.mm
        ${POMDOG_SRC_DIR}/Platform.Cocoa/GameHostCocoa.hpp
        ${POMDOG_SRC_DIR}/Platform.Cocoa/GameHostCocoa.mm
        ${POMDOG_SRC_DIR}/Platform.Cocoa/GameWindowCocoa.hpp
        ${POMDOG_SRC_DIR}/Platform.Cocoa/GameWindowCocoa.mm
        ${POMDOG_SRC_DIR}/Platform.Cocoa/KeyboardCocoa.hpp
        ${POMDOG_SRC_DIR}/Platform.Cocoa/KeyboardCocoa.cpp
        ${POMDOG_SRC_DIR}/Platform.Cocoa/MouseCocoa.hpp
        ${POMDOG_SRC_DIR}/Platform.Cocoa/MouseCocoa.cpp
        ${POMDOG_SRC_DIR}/Platform.Cocoa/OpenGLContextCocoa.hpp
        ${POMDOG_SRC_DIR}/Platform.Cocoa/OpenGLContextCocoa.mm
        ${POMDOG_SRC_DIR}/Platform.Cocoa/PomdogOpenGLView.hpp
        ${POMDOG_SRC_DIR}/Platform.Cocoa/PomdogOpenGLView.mm
    >

    $<$<PLATFORM_ID:Linux>:
        # NOTE: Platform.Linux
        ${POMDOG_SRC_DIR}/Platform.Linux/FileSystemLinux.cpp
        ${POMDOG_SRC_DIR}/Platform.Linux/TimeSourceLinux.cpp
        ${POMDOG_SRC_DIR}/Platform.Linux/TimeSourceLinux.hpp
    >

    $<$<PLATFORM_ID:Linux>:
        # NOTE: Platform.X11
        ${POMDOG_INC_DIR}/Platform/X11/Bootstrap.hpp
        ${POMDOG_SRC_DIR}/Platform.X11/Bootstrap.cpp
        ${POMDOG_SRC_DIR}/Platform.X11/GameHostX11.cpp
        ${POMDOG_SRC_DIR}/Platform.X11/GameHostX11.hpp
        ${POMDOG_SRC_DIR}/Platform.X11/GameWindowX11.cpp
        ${POMDOG_SRC_DIR}/Platform.X11/GameWindowX11.hpp
        ${POMDOG_SRC_DIR}/Platform.X11/KeyboardX11.cpp
        ${POMDOG_SRC_DIR}/Platform.X11/KeyboardX11.hpp
        ${POMDOG_SRC_DIR}/Platform.X11/MouseX11.cpp
        ${POMDOG_SRC_DIR}/Platform.X11/MouseX11.hpp
        ${POMDOG_SRC_DIR}/Platform.X11/OpenGLContextX11.cpp
        ${POMDOG_SRC_DIR}/Platform.X11/OpenGLContextX11.hpp
        ${POMDOG_SRC_DIR}/Platform.X11/X11AtomCache.hpp
        ${POMDOG_SRC_DIR}/Platform.X11/X11Context.cpp
        ${POMDOG_SRC_DIR}/Platform.X11/X11Context.hpp
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: Platform.Win32
        ${POMDOG_SRC_DIR}/Platform.Win32/Bootstrap.cpp
        ${POMDOG_INC_DIR}/Platform/Win32/Bootstrap.hpp
        ${POMDOG_INC_DIR}/Platform/Win32/BootstrapSettingsWin32.hpp
        ${POMDOG_SRC_DIR}/Platform.Win32/GameHostWin32.cpp
        ${POMDOG_SRC_DIR}/Platform.Win32/GameHostWin32.hpp
        ${POMDOG_SRC_DIR}/Platform.Win32/GameWindowWin32.cpp
        ${POMDOG_SRC_DIR}/Platform.Win32/GameWindowWin32.hpp
        ${POMDOG_SRC_DIR}/Platform.Win32/FileSystemWin32.cpp
        ${POMDOG_SRC_DIR}/Platform.Win32/KeyboardWin32.cpp
        ${POMDOG_SRC_DIR}/Platform.Win32/KeyboardWin32.hpp
        ${POMDOG_SRC_DIR}/Platform.Win32/MouseWin32.cpp
        ${POMDOG_SRC_DIR}/Platform.Win32/MouseWin32.hpp
        ${POMDOG_INC_DIR}/Platform/Win32/PrerequisitesWin32.hpp
        ${POMDOG_SRC_DIR}/Platform.Win32/TimeSourceWin32.cpp
        ${POMDOG_SRC_DIR}/Platform.Win32/TimeSourceWin32.hpp
    >

    $<$<AND:$<PLATFORM_ID:Windows>,$<BOOL:${POMDOG_USE_GL4}>>:
        # NOTE: Platform.Win32
        ${POMDOG_SRC_DIR}/Platform.Win32/OpenGLContextWin32.cpp
        ${POMDOG_SRC_DIR}/Platform.Win32/OpenGLContextWin32.hpp
    >
)
