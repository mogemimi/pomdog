//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_BOOTSTRAPPERWIN32_4778D685_22F5_4D2E_B7D5_5D8F47760747_HPP
#define POMDOG_BOOTSTRAPPERWIN32_4778D685_22F5_4D2E_B7D5_5D8F47760747_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#ifndef STRICT
#	define STRICT
#endif
#ifndef	WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN
#endif
#if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX
#endif

#include <Windows.h>
#include <memory>
#include <functional>

namespace Pomdog {

class GameHost;

namespace Details {
namespace Win32 {

class GameWindowWin32;

class BootstrapperWin32 {
public:
	BootstrapperWin32(HINSTANCE hInstance, int nCmdShow, int width, int height, bool isFullscreen);

	void Run(std::function<void(std::shared_ptr<GameHost> const& gameHost)> const& run);

private:
	HINSTANCE hInstance;
	int nCmdShow;
	int width;
	int height;
	bool isFullscreen;
};

}// namespace Win32
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_BOOTSTRAPPERWIN32_4778D685_22F5_4D2E_B7D5_5D8F47760747_HPP)
