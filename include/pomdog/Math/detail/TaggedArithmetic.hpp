//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DETAIL_TAGGEDARITHMETIC_HPP
#define POMDOG_DETAIL_TAGGEDARITHMETIC_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"
#include "../Utility/Assert.hpp"
#include <type_traits>

namespace Pomdog {
namespace Details {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

template <typename T, class PhantomType>
class TaggedArithmetic
{
public:
	static_assert(std::is_arithmetic<T>::value, "T is arithmetic type.");

	T value;

public:
	// Constructors:
	TaggedArithmetic() = default;
	TaggedArithmetic(TaggedArithmetic const&) = default;
	TaggedArithmetic(TaggedArithmetic &&) = default;

	explicit TaggedArithmetic(T v)
		: value(v){};

	virtual ~TaggedArithmetic() = default;

	// Assignment operators:
	TaggedArithmetic & operator=(TaggedArithmetic const&) = default;
	TaggedArithmetic & operator=(TaggedArithmetic &&) = default;

	TaggedArithmetic& operator+=(TaggedArithmetic const& v)
	{
		value += v.value;
		return *this;
	}

	TaggedArithmetic& operator-=(TaggedArithmetic const& v)
	{
		value -= v.value;
		return *this;
	}

	TaggedArithmetic& operator*=(TaggedArithmetic const& v)
	{
		value *= v.value;
		return *this;
	}

	TaggedArithmetic & operator/=(TaggedArithmetic const& v)
	{
		POMDOG_ASSERT(v.value != 0);
		value /= v.value;
		return *this;
	}

	TaggedArithmetic & operator*=(T scaleFactor)
	{
		value *= scaleFactor;
		return *this;
	}

	TaggedArithmetic & operator/=(T scaleFactor)
	{
		POMDOG_ASSERT(scaleFactor != 0);
		value /= scaleFactor;
		return *this;
	}

	// Unary operators:
	TaggedArithmetic operator+() const
	{
		return TaggedArithmetic(+value);
	}

	TaggedArithmetic operator-() const
	{
		return TaggedArithmetic(-value);
	}

	// Binary operators:
	TaggedArithmetic operator+(TaggedArithmetic const& v) const
	{
		return TaggedArithmetic(value + v.value);
	}

	TaggedArithmetic operator-(TaggedArithmetic const& v) const
	{
		return TaggedArithmetic(value - v.value);
	}

	TaggedArithmetic operator*(TaggedArithmetic const& v) const
	{
		return TaggedArithmetic(value * v.value);
	}

	TaggedArithmetic operator/(TaggedArithmetic const& v) const
	{
		POMDOG_ASSERT(v.value != static_cast<T>(0));
		return TaggedArithmetic(value / v.value);
	}

	TaggedArithmetic operator*(T scaleFactor) const
	{
		return TaggedArithmetic(value * scaleFactor);
	}

	TaggedArithmetic operator/(T scaleFactor) const
	{
		POMDOG_ASSERT(scaleFactor != static_cast<T>(0));
		return TaggedArithmetic(value / scaleFactor);
	}

	friend TaggedArithmetic operator*(T scaleFactor, TaggedArithmetic const& v)
	{
		return TaggedArithmetic(scaleFactor * v.value);
	}

	friend TaggedArithmetic operator/(T scaleFactor, TaggedArithmetic const& v)
	{
		POMDOG_ASSERT(v.value != static_cast<T>(0));
		return TaggedArithmetic(scaleFactor / v.value);
	}

	bool operator==(TaggedArithmetic const& v) const
	{
		return value == v.value;
	}

	bool operator!=(TaggedArithmetic const& v) const
	{
		return value != v.value;
	}

	bool operator==(T scalar) const
	{
		return value == scalar;
	}

	bool operator!=(T scalar) const
	{
		return value != scalar;
	}

	bool operator<(TaggedArithmetic const& v) const
	{
		return value < v.value;
	}

	bool operator>(TaggedArithmetic const& v) const
	{
		return value > v.value;
	}

	bool operator<=(TaggedArithmetic const& v) const
	{
		return value <= v.value;
	}

	bool operator>=(TaggedArithmetic const& v) const
	{
		return value >= v.value;
	}

	bool operator<(T scalar) const
	{
		return value < scalar;
	}

	bool operator>(T scalar) const
	{
		return value > scalar;
	}

	bool operator<=(T scalar) const
	{
		return value <= scalar;
	}

	bool operator>=(T scalar) const
	{
		return value >= scalar;
	}
};

/// @}
/// @}

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_TAGGEDARITHMETIC_HPP)
