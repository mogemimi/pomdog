// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Resource.hpp"
#include "../TestAppGame.hpp"
#include "Pomdog/Platform/Win32/BootstrapperWin32.hpp"
#include "Pomdog/Pomdog.hpp"

#if (defined(_DEBUG) || defined(DEBUG)) && !defined(NDEBUG)
#	define POMDOG_CRTDEBUG
#endif

#if defined (POMDOG_CRTDEBUG)
#	define _CRTDBG_MAP_ALLOC
#	include <cstdlib>
#	include <crtdbg.h>
#endif

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#if defined (POMDOG_CRTDEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_EVERY_1024_DF);
#endif

	using namespace Pomdog;

#if defined(DEBUG) && !defined(NDEBUG)
	ScopedConnection connection = Log::Connect([](LogEntry const& entry) {
		OutputDebugString(entry.Message.c_str());
		OutputDebugString("\n");
	});
	Log::SetLevel(Pomdog::LogLevel::Internal);
#endif

	Detail::Win32::BootstrapSettingsWin32 settings;
	settings.HInstance = hInstance;
	settings.CmdShow = nCmdShow;
	settings.Icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_POMDOG_ICON));
	settings.IconSmall = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_POMDOG_ICON_SMALL));
	settings.Width = 800;
	settings.Height = 480;
	settings.IsFullscreen = false;

	Detail::Win32::BootstrapperWin32 bootstrapper(settings);

	bootstrapper.Run([](std::shared_ptr<GameHost> const& gameHost)
	{
		try {
			TestApp::TestAppGame game(gameHost);
			gameHost->Run(game);
		}
		catch (std::exception const& e) {
			Log::Critical("Pomdog", e.what());
			_CrtDbgBreak();
		}
	});
	return 0;
}
