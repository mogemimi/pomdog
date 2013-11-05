//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SCOPEDCONNECTION_H
#define POMDOG_SCOPEDCONNECTION_H

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <type_traits>
#include <utility>

namespace Pomdog {

template <typename T>
class ScopedConnection
{
public:
	ScopedConnection() = default;
	ScopedConnection(ScopedConnection const&) = delete;
	ScopedConnection(ScopedConnection &&) = default;

	explicit ScopedConnection(T const& connection)
		: connection(connection)
	{}
	
	explicit ScopedConnection(T && connection)
		: connection(std::move(connection))
	{}
	
	virtual ~ScopedConnection()
	{
		connection.Disconnect();
	}

	void Reset()
	{
		connection.Disconnect();
	}

	void Reset(T const& c)
	{
		connection.Disconnect();
		connection = c;
	}
	
	void Reset(T && c)
	{
		connection.Disconnect();
		connection = std::move(c);
	}

private:
	static_assert(std::is_class<T>::value, "");
	T connection;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SCOPEDCONNECTION_H)
