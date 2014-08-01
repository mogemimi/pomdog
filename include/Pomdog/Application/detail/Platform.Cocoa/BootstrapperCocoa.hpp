//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DETAIL_BOOTSTRAPPERCOCOA_13B44EFA_9FFA_4AE2_953A_22EFEB4FC322_HPP
#define POMDOG_DETAIL_BOOTSTRAPPERCOCOA_13B44EFA_9FFA_4AE2_953A_22EFEB4FC322_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#import <Cocoa/Cocoa.h>

#include "Pomdog/Config/Export.hpp"
#include "Pomdog/Application/GameHost.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Logging/LogLevel.hpp"
#include <type_traits>
#include <memory>
#include <array>
#include <functional>
#include <exception>


@class NSWindow;

namespace Pomdog {

class Game;
class GameHost;

namespace Details {
namespace Cocoa {

class CocoaGameWindow;

///@~Japanese
/// @brief すべてのサブシステムの起動、およびアプリケーションの実行を行います。
class POMDOG_EXPORT BootstrapperCocoa final {
public:
	BootstrapperCocoa() = default;
	
	template <class GameClass>
	void Run(NSWindow* nativeWindow)
	{
		static_assert(std::is_base_of<Game, GameClass>::value, "GameClass is base of Pomdog::Game.");
		static_assert(!std::is_abstract<GameClass>::value, "GameClass is not abstract.");
		
		BeginRun(nativeWindow);
		
		try {
			auto game = std::make_shared<GameClass>(gameHost);
			gameHost->Run(game);
		}
		catch (std::exception const& e) {
			Log::Critical("Pomdog", e.what());
		}
		
		EndRun();
	}
	
private:
	std::shared_ptr<CocoaGameWindow> gameWindow;
	std::shared_ptr<GameHost> gameHost;
	
private:
	void BeginRun(NSWindow* nativeWindow);
	void EndRun();
};

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_BOOTSTRAPPERCOCOA_13B44EFA_9FFA_4AE2_953A_22EFEB4FC322_HPP)
