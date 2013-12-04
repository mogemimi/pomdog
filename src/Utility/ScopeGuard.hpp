//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_SCOPEGUARD_HPP
#define POMDOG_SRC_SCOPEGUARD_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Utility/Noncopyable.hpp>
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

#endif // !defined(POMDOG_SRC_SCOPEGUARD_HPP)
