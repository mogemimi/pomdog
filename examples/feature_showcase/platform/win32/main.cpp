#include "game_setup.h"
#include "resource.h"
#include "pomdog/platform/win32/bootstrap_win32.h"
#include "pomdog/pomdog.h"

#if defined(POMDOG_DEBUG_BUILD)
#define POMDOG_CRTDEBUG
#endif

#if defined(POMDOG_CRTDEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <cstdlib>
#endif

int WINAPI WinMain(
    HINSTANCE hInstance,
    [[maybe_unused]] HINSTANCE hPrevInstance,
    [[maybe_unused]] LPSTR lpCmdLine,
    int nCmdShow)
{
#if defined(POMDOG_CRTDEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_EVERY_1024_DF);
#endif

    using namespace pomdog;

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    ScopedConnection connection = Log::Connect([](LogEntry const& entry) {
        OutputDebugString(entry.Message.c_str());
        OutputDebugString("\n");
    });
    Log::SetLevel(pomdog::LogLevel::Verbose);
#endif

    win32::Bootstrap bootstrap;
    bootstrap.setInstance(hInstance);
    bootstrap.setCommandShow(nCmdShow);
    bootstrap.setIcon(LoadIcon(hInstance, MAKEINTRESOURCE(IDI_POMDOG_ICON)));
    bootstrap.setIconSmall(LoadIcon(hInstance, MAKEINTRESOURCE(IDI_POMDOG_ICON_SMALL)));
    bootstrap.setCommandLineArgs(__argc, const_cast<const char* const*>(__argv));

    bootstrap.onError([](std::unique_ptr<Error>&& err) {
        Log::Critical("pomdog", err->toString());
#if defined(POMDOG_CRTDEBUG)
        _CrtDbgBreak();
#endif
    });

    bootstrap.run(feature_showcase::createGameSetup());

    return 0;
}
