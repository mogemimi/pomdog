//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "CocoaGameHost.hpp"

namespace Pomdog {
namespace Details {
namespace Cocoa {

//-----------------------------------------------------------------------
CocoaGameHost::CocoaGameHost()
	: exitRequest(false)
{
	systemHub = std::make_shared<Details::SystemHub>();
	Details::ResetGlobalSystemHub(systemHub);
}
//-----------------------------------------------------------------------
CocoaGameHost::~CocoaGameHost()
{
	Details::ResetGlobalSystemHub();
}
//-----------------------------------------------------------------------
void CocoaGameHost::Run()
{
	///@todo Not implemented
	/// Insert code here to run your application
}
//-----------------------------------------------------------------------
void CocoaGameHost::Exit()
{
	exitRequest = true;
}

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog
