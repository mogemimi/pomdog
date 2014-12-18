//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MATH_DETAIL_COORDINATE3D_6062BA6C_8D59_4807_B118_EB89C4A3A3A3_HPP
#define POMDOG_MATH_DETAIL_COORDINATE3D_6062BA6C_8D59_4807_B118_EB89C4A3A3A3_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Config/Export.hpp"
#include <type_traits>

namespace Pomdog {
namespace Details {

template <typename T>
class POMDOG_EXPORT Coordinate3D final {
public:
	static_assert(std::is_arithmetic<T>::value, "You can only use arithmetic types.");
	typedef T value_type;

	T X, Y, Z;

public:
	// Constructors:
	Coordinate3D() = default;
	
	///@brief Copy constructor.
	Coordinate3D(Coordinate3D const&) = default;
	
	///@brief Move constructor.
	Coordinate3D(Coordinate3D &&) = default;
	
	///@brief Construct from T values.
	Coordinate3D(T x, T y, T z);
	
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
	
	static Coordinate3D const Zero;
};

template <typename T>
Coordinate3D<T> POMDOG_EXPORT
operator*(T scaleFactor, Coordinate3D<T> const& coordinate);

}// namespace Details
}// namespace Pomdog

#include "Coordinate3DImplementation.hpp"

#endif // !defined(POMDOG_MATH_DETAIL_COORDINATE3D_6062BA6C_8D59_4807_B118_EB89C4A3A3A3_HPP)
