//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Event/EventConnection.hpp>

namespace Pomdog {
//-----------------------------------------------------------------------
void EventConnection::Disconnect()
{
	if (auto locked_body = weakConnectionBody.lock()) {
		locked_body->Disconnect();
	}
}

}// namespace Pomdog
