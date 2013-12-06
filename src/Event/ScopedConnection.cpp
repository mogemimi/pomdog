//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Event/ScopedConnection.hpp>
#include <utility>

namespace Pomdog {
//-----------------------------------------------------------------------
ScopedConnection::ScopedConnection(EventConnection const& connection)
	: connection(connection)
{}
//-----------------------------------------------------------------------
ScopedConnection::ScopedConnection(EventConnection && connection)
	: connection(std::move(connection))
{}
//-----------------------------------------------------------------------
ScopedConnection::~ScopedConnection()
{
	connection.Disconnect();
}
//-----------------------------------------------------------------------
ScopedConnection & ScopedConnection::operator=(EventConnection const& c)
{
	connection.Disconnect();
	connection = c;
	return *this;
}
//-----------------------------------------------------------------------
ScopedConnection & ScopedConnection::operator=(EventConnection && c)
{
	connection.Disconnect();
	connection = std::move(c);
	return *this;
}
//-----------------------------------------------------------------------
void ScopedConnection::Disconnect()
{
	connection.Disconnect();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
