//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DETAIL_BOOTSTRAPPERCOCOA_HPP
#define POMDOG_DETAIL_BOOTSTRAPPERCOCOA_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Config/FundamentalTypes.hpp>
#include <memory>
#include <type_traits>
#include <array>
#include <functional>
#include <Pomdog/Application/GameHost.hpp>

#import <Cocoa/Cocoa.h>

@class NSWindow;

namespace Pomdog {

class Game;
class GameHost;
class GameWindow;

namespace Details {
namespace Cocoa {

/// @addtogroup Framework
/// @{
/// @addtogroup Application
/// @{

///@~Japanese
/// @brief すべてのサブシステムをの起動、およびアプリケーションの実行を行います。
class BootstrapperCocoa final
{
private:
	std::shared_ptr<GameHost> gameHost;
	std::shared_ptr<GameWindow> gameWindow;

public:
	BootstrapperCocoa();
	
	void BuildOpenGLView(NSWindow* nativeWindow);
	
	template <class GameClass>
	void Run()
	{
		static_assert(std::is_base_of<Game, GameClass>::value, "GameClass is base of Pomdog::Game.");
		static_assert(!std::is_abstract<GameClass>::value, "GameClass is not abstract.");
		
		auto game = std::make_shared<GameClass>(*gameHost);
		gameHost->Run(game);
	}
};

/// @}
/// @}

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_BOOTSTRAPPER_HPP)
