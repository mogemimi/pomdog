#include "Resource.hpp"
#include "../Source/FeatureShowcaseGame.hpp"
#include <Pomdog/Platform/Win32/Bootstrap.hpp>
#include <Pomdog/Pomdog.hpp>

#if defined(DEBUG) && !defined(NDEBUG)
#define POMDOG_CRTDEBUG
#endif

#if defined(POMDOG_CRTDEBUG)
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#endif

int APIENTRY _tWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

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

    bootstrap.OnError([](std::exception const& e) {
        Log::Critical("Pomdog", e.what());
#if defined(POMDOG_CRTDEBUG)
        _CrtDbgBreak();
#endif
    });

    bootstrap.Run([](std::shared_ptr<GameHost> const& gameHost) {
        return std::make_unique<FeatureShowcase::FeatureShowcaseGame>(gameHost);
    });

    return 0;
}
