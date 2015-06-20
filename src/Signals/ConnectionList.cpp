// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Signals/ConnectionList.hpp"

namespace Pomdog {

void ConnectionList::Disconnect()
{
    connections.clear();
}

} // namespace Pomdog
