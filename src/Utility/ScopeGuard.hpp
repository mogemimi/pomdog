//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SRC_UTILITY_SCOPEGUARD_1F749886_7638_4384_8510_50A4E4B6CE16_HPP
#define POMDOG_SRC_UTILITY_SCOPEGUARD_1F749886_7638_4384_8510_50A4E4B6CE16_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <Pomdog/Utility/Assert.hpp>
#include <functional>
#include <utility>

namespace Pomdog {
namespace Details {

class ScopeGuard final {
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
