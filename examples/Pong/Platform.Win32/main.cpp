#include "Resource.hpp"
#include "../Source/PongGame.hpp"
#include <Pomdog/Platform/Win32/Bootstrap.hpp>
#include <Pomdog/Pomdog.hpp>

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
    [[maybe_unsed]] HINSTANCE hPrevInstance,
    [[maybe_unsed]] LPSTR lpCmdLine,
    int nCmdShow)
{
#if defined(POMDOG_CRTDEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_EVERY_1024_DF);
#endif

    using namespace Pomdog;

#if defined(DEBUG) && !defined(NDEBUG)
    ScopedConnection connection = Log::Connect([](LogEntry const& entry) {
        OutputDebugString(entry.Message.c_str());
        OutputDebugString("\n");
    });
    Log::SetLevel(Pomdog::LogLevel::Verbose);
#endif

    Win32::Bootstrap bootstrap;
    bootstrap.SetInstance(hInstance);
    bootstrap.SetCommandShow(nCmdShow);
    bootstrap.SetIcon(LoadIcon(hInstance, MAKEINTRESOURCE(IDI_POMDOG_ICON)));
    bootstrap.SetIconSmall(LoadIcon(hInstance, MAKEINTRESOURCE(IDI_POMDOG_ICON_SMALL)));
    bootstrap.SetBackBufferSize(800, 480);
    bootstrap.SetOpenGLEnabled(false);

    bootstrap.OnError([](std::unique_ptr<Error>&& err) {
        Log::Critical("Pomdog", err->ToString());
#if defined(POMDOG_CRTDEBUG)
        _CrtDbgBreak();
#endif
    });

    bootstrap.Run([](std::shared_ptr<GameHost> const& gameHost) {
        return std::make_unique<Pong::PongGame>(gameHost);
    });

    return 0;
}
