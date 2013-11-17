//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_COORDINATE3D_HPP
#define POMDOG_COORDINATE3D_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"
#include <type_traits>

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

template <typename T>
class Coordinate3D final
{
public:
	static_assert(std::is_arithmetic<T>::value, "You can only use arithmetic types.");
	typedef T value_type;

	T x, y, z;

public:
	// Constructors:
	Coordinate3D() = default;
	
	///@brief Copy constructor.
	Coordinate3D(Coordinate3D const&) = default;
	
	///@brief Move constructor.
	Coordinate3D(Coordinate3D &&) = default;
	
	///@brief Construct from T values.
	Coordinate3D(T x, T y, T z);
	
	~Coordinate3D() = default;

	// Assignment operators:
	Coordinate3D & operator=(Coordinate3D const&) = default;
	Coordinate3D & operator=(Coordinate3D &&) = default;
	Coordinate3D & operator+=(Coordinate3D const&);
	Coordinate3D & operator-=(Coordinate3D const&);
	Coordinate3D & operator*=(T scaleFactor);
	Coordinate3D & operator/=(T scaleFactor);

	// Unary operators:
	Coordinate3D operator+() const;
	Coordinate3D operator-() const;

	// Binary operators:
	Coordinate3D operator+(Coordinate3D const&) const;
	Coordinate3D operator-(Coordinate3D const&) const;
	Coordinate3D operator*(Coordinate3D const&) const;
	Coordinate3D operator/(Coordinate3D const&) const;
	Coordinate3D operator*(T scaleFactor) const;
	Coordinate3D operator/(T scaleFactor) const;

	bool operator==(Coordinate3D const&) const;
	bool operator!=(Coordinate3D const&) const;
};

template <typename T>
Coordinate3D<T> operator*(T scaleFactor, Coordinate3D<T> const& coordinate);

/// @}
/// @}

}// namespace Pomdog

#include "detail/Coordinate3DImplementation.hpp"

#endif // !defined(POMDOG_COORDINATE3D_HPP)
