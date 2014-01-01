//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_UTILITY_SCOPEGUARD_1F749886_7638_4384_8510_50A4E4B6CE16_HPP
#define POMDOG_SRC_UTILITY_SCOPEGUARD_1F749886_7638_4384_8510_50A4E4B6CE16_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <functional>
#include <utility>
#include <Pomdog/Utility/Assert.hpp>

namespace Pomdog {
namespace Details {

class ScopeGuard final
{
public:
	ScopeGuard() = delete;
	ScopeGuard(ScopeGuard const&) = delete;
	ScopeGuard(ScopeGuard &&) = default;
	
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
	
	ScopeGuard & operator=(ScopeGuard const&) = delete;
	ScopeGuard & operator=(ScopeGuard &&) = default;

private:
	std::function<void()> func;
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_UTILITY_SCOPEGUARD_1F749886_7638_4384_8510_50A4E4B6CE16_HPP)
