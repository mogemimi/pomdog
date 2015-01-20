//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SCOPEDCONNECTION_009320AB_E09C_4157_8F9D_E7FBCE812DD8_HPP
#define POMDOG_SCOPEDCONNECTION_009320AB_E09C_4157_8F9D_E7FBCE812DD8_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "EventConnection.hpp"
#include "Pomdog/Basic/Export.hpp"

namespace Pomdog {

class POMDOG_EXPORT ScopedConnection final {
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

#endif // !defined(POMDOG_SCOPEDCONNECTION_009320AB_E09C_4157_8F9D_E7FBCE812DD8_HPP)
