//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SCOPEGUARD_H
#define POMDOG_SCOPEGUARD_H

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <pomdog/Utility/Assert.hpp>
#include <pomdog/Utility/Noncopyable.hpp>
#include <functional>
#include <utility>

namespace Pomdog {
namespace Details {

class ScopeGuard final: Noncopyable
{
public:
	explicit ScopeGuard(std::function<void()> const& f)
		: func(f)
	{}

	explicit ScopeGuard(std::function<void()> && f)
		: func(std::move(f))
	{}

	~ScopeGuard()
	{
		POMDOG_ASSERT(func);
		func();
	}

private:
	std::function<void()> func;
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SCOPEGUARD_H)
