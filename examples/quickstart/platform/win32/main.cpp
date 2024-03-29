#include "resource.h"
#include "game_main.h"
#include "pomdog/platform/win32/bootstrap_win32.h"
#include "pomdog/pomdog.h"

#if defined(DEBUG) && !defined(NDEBUG)
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

#if defined(DEBUG) && !defined(NDEBUG)
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
    bootstrap.setBackBufferSize(800, 480);
    bootstrap.setOpenGLEnabled(false);

    bootstrap.onError([](std::unique_ptr<Error>&& err) {
        Log::Critical("pomdog", err->toString());
#if defined(POMDOG_CRTDEBUG)
        _CrtDbgBreak();
#endif
    });

    bootstrap.run([](std::shared_ptr<GameHost> const& gameHost) {
        return std::make_unique<quickstart::GameMain>(gameHost);
    });

    return 0;
}
