//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_COCOA_GAMEHOST_HPP
#define POMDOG_COCOA_GAMEHOST_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Application/GameHost.hpp>
#include <Pomdog/Application/detail/SystemHub.hpp>
#include <memory>

namespace Pomdog {
namespace Details {
namespace Cocoa {

class CocoaGameHost final: public GameHost
{
public:
	CocoaGameHost();

	~CocoaGameHost();

	void Run() override;
	
	void Exit() override;
	
private:
	std::shared_ptr<Details::SystemHub> systemHub;
	bool exitRequest;
};

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_COCOA_GAMEHOST_HPP)
