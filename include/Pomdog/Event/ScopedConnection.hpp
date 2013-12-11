//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SCOPEDCONNECTION_8CA9B8CF_5D85_11E3_807D_A8206655A22B_HPP
#define POMDOG_SCOPEDCONNECTION_8CA9B8CF_5D85_11E3_807D_A8206655A22B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/Export.hpp"
#include "detail/FowardDeclarations.hpp"
#include "EventConnection.hpp"

namespace Pomdog {

class POMDOG_EXPORT ScopedConnection final
{
private:
	EventConnection connection;

public:
	ScopedConnection() = default;
	ScopedConnection(ScopedConnection const&) = delete;
	ScopedConnection(ScopedConnection &&) = default;
	
	ScopedConnection(EventConnection const& connection);
	ScopedConnection(EventConnection && connection);
	
	~ScopedConnection();
	
	ScopedConnection & operator=(ScopedConnection const&) = delete;
	ScopedConnection & operator=(ScopedConnection &&) = default;

	ScopedConnection & operator=(EventConnection const& c);
	ScopedConnection & operator=(EventConnection && c);

	void Disconnect();
};

}// namespace Pomdog

#endif // !defined(POMDOG_SCOPEDCONNECTION_8CA9B8CF_5D85_11E3_807D_A8206655A22B_HPP)
