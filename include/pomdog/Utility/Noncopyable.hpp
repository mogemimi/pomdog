//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_NONCOPYABLE_HPP
#define POMDOG_NONCOPYABLE_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/Platform.hpp"

namespace Pomdog {

#if ((defined(POMDOG_COMPILER_MSVC) && (_MSC_VER > 1700)) ||\
	(defined(POMDOG_COMPILER_CLANG) && __has_feature(cxx_defaulted_functions)) ||\
	(defined(POMDOG_COMPILER_GNUC) && defined(__GXX_EXPERIMENTAL_CXX0X__)))
// C++11 or later
class Noncopyable
{
public:
	Noncopyable() = default;
	Noncopyable(Noncopyable const&) = delete;
	Noncopyable& operator=(Noncopyable const&) = delete;
};

#else
// C++03
class Noncopyable
{
public:
	Noncopyable() {}
	~Noncopyable() {}

private:
	// hidden copy constructor and assignment operator. 
	Noncopyable(Noncopyable const&);
	Noncopyable const& operator=(Noncopyable const&);
};

#endif

}// namespace Pomdog

#endif // !defined(POMDOG_NONCOPYABLE_HPP)
