//
//  Copyright (C) 2013 the Pomdog Engine authors.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_RADIAN_H
#define POMDOG_RADIAN_H

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"
#include "detail/TaggedArithmetic.hpp"

namespace Pomdog {
namespace Details {
namespace Tags {

struct radian_tag {};

}// namespace Tags
}// namespace Details

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

#if !defined(_MSC_VER) || (_MSC_VER >= 1800)
//
// C++11 (Templates aliases supported)
//
template <typename T>
using Radian = Details::TaggedArithmetic<T, Details::Tags::radian_tag, std::is_floating_point<T>::value>;

#else
//
// C++03 (Templates aliases not supported)
// See also:
// http://msdn.microsoft.com/en-us/library/hh567368.aspx
//
template <typename T>
class Radian: public Details::TaggedArithmetic<T, Details::Tags::radian_tag, std::is_floating_point<T>::value>
{
	Radian(){}
	
	explicit Radian(T v): TaggedArithmetic(v){}

	Radian(TaggedArithmetic const & v): TaggedArithmetic(v){}

	Radian & operator=(TaggedArithmetic const & v)
		{ value += v.value; return *this; }
		
	virtual ~Radian(){}
};

#endif // !defined(_MSC_VER)

}// namespace Pomdog

#endif // !defined(POMDOG_RADIAN_H)
