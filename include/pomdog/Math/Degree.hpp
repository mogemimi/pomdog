//
//  Copyright (C) 2013 the Pomdog Engine authors.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DEGREE_HPP
#define POMDOG_DEGREE_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"
#include "detail/TaggedArithmetic.hpp"

namespace Pomdog {
namespace Details {
namespace Tags {

struct degree_tag {};

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
using Degree = Details::TaggedArithmetic<T, Details::Tags::degree_tag, std::is_floating_point<T>::value>;

#else
//
// C++03 (Templates aliases not supported)
// See also:
// http://msdn.microsoft.com/en-us/library/hh567368.aspx
//
template <typename T>
struct Degree : public Details::TaggedArithmetic<T, Details::Tags::degree_tag, std::is_floating_point<T>::value>
{
	Degree(){};
	
	explicit Degree(T v): TaggedArithmetic(v){}

	Degree(TaggedArithmetic const & v): TaggedArithmetic(v){}

	Degree & operator=(TaggedArithmetic const & v)
		{ value += v.value; return *this; }
};

#endif // !defined(_MSC_VER)

}// namespace Pomdog

#endif // !defined(POMDOG_DEGREE_HPP)
