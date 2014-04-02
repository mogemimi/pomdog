//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MATH_DETAIL_COORDINATE2D_9F61E901_0DBD_451E_B518_E52D38B77A9E_HPP
#define POMDOG_MATH_DETAIL_COORDINATE2D_9F61E901_0DBD_451E_B518_E52D38B77A9E_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <type_traits>
#include "../../Config/Export.hpp"

namespace Pomdog {
namespace Details {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

template <typename T>
class POMDOG_EXPORT Coordinate2D final {
public:
	static_assert(std::is_arithmetic<T>::value, "You can only use arithmetic types.");
	typedef T value_type;

	T X, Y;

public:
	// Constructors:
	Coordinate2D() = default;
	
	///@brief Copy constructor.
	Coordinate2D(Coordinate2D const&) = default;
	
	///@brief Move constructor.
	Coordinate2D(Coordinate2D &&) = default;
	
	///@brief Construct from T values.
	Coordinate2D(T x, T y);
	
	// Assignment operators:
	Coordinate2D & operator=(Coordinate2D const&) = default;
	Coordinate2D & operator=(Coordinate2D &&) = default;
	Coordinate2D & operator+=(Coordinate2D const&);
	Coordinate2D & operator-=(Coordinate2D const&);
	Coordinate2D & operator*=(T scaleFactor);
	Coordinate2D & operator/=(T scaleFactor);

	// Unary operators:
	Coordinate2D operator+() const;
	Coordinate2D operator-() const;

	// Binary operators:
	Coordinate2D operator+(Coordinate2D const&) const;
	Coordinate2D operator-(Coordinate2D const&) const;
	Coordinate2D operator*(Coordinate2D const&) const;
	Coordinate2D operator/(Coordinate2D const&) const;
	Coordinate2D operator*(T scaleFactor) const;
	Coordinate2D operator/(T scaleFactor) const;

	bool operator==(Coordinate2D const&) const;
	bool operator!=(Coordinate2D const&) const;
	
	static Coordinate2D const Zero;
};

template <typename T>
Coordinate2D<T> operator*(T scaleFactor, Coordinate2D<T> const& coordinate);

/// @}
/// @}

}// namespace Details
}// namespace Pomdog

#include "Coordinate2DImplementation.hpp"

#endif // !defined(POMDOG_MATH_DETAIL_COORDINATE2D_9F61E901_0DBD_451E_B518_E52D38B77A9E_HPP)
