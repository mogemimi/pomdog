//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_DETAIL_BOOTSTRAPPERCOCOA_13B44EFA_9FFA_4AE2_953A_22EFEB4FC322_HPP
#define POMDOG_DETAIL_BOOTSTRAPPERCOCOA_13B44EFA_9FFA_4AE2_953A_22EFEB4FC322_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Basic/Export.hpp"
#import <Cocoa/Cocoa.h>
#include <memory>
#include <functional>

@class NSWindow;

namespace Pomdog {

class GameHost;

namespace Details {
namespace Cocoa {

///@~Japanese
/// @brief すべてのサブシステムの起動、およびアプリケーションの実行を行います。
class POMDOG_EXPORT BootstrapperCocoa final {
public:
	void Run(NSWindow* nativeWindow, std::function<void(std::shared_ptr<GameHost> const&)> const& runGame);
};

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_BOOTSTRAPPERCOCOA_13B44EFA_9FFA_4AE2_953A_22EFEB4FC322_HPP)
