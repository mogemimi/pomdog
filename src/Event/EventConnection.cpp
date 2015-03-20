// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Event/EventConnection.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
EventConnection::EventConnection(EventConnection const& connection)
{
	if (connection.body) {
		body = connection.body->DeepCopy();
	}
}
//-----------------------------------------------------------------------
EventConnection & EventConnection::operator=(EventConnection const& connection)
{
	if (connection.body) {
		body = connection.body->DeepCopy();
	}
	return *this;
}
//-----------------------------------------------------------------------
void EventConnection::Disconnect()
{
	if (body) {
		body->Disconnect();
	}
}
//-----------------------------------------------------------------------
}// namespace Pomdog
