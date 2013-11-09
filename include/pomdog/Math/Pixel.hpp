//
//  Copyright (C) 2013 the Pomdog Engine authors.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PIXEL_HPP
#define POMDOG_PIXEL_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"
#include "detail/TaggedArithmetic.hpp"

namespace Pomdog {
namespace Details {
namespace Tags {

struct pixel_tag {};

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
using Pixel = Details::TaggedArithmetic<T, Details::Tags::pixel_tag>;

#else
//
// C++03 (Templates aliases not supported)
// See also:
// http://msdn.microsoft.com/en-us/library/hh567368.aspx
//
template <typename T>
struct Pixel : public Details::TaggedArithmetic<T, Details::Tags::pixel_tag>
{
	Pixel(){};
	explicit Pixel(T v): TaggedArithmetic(v){}

	Pixel(TaggedArithmetic const & v): TaggedArithmetic(v){}

	Pixel & operator=(TaggedArithmetic const & v)
		{ value += v.value; return *this; }
};

#endif // !defined(_MSC_VER)

}// namespace Pomdog

#endif // !defined(POMDOG_PIXEL_HPP)
