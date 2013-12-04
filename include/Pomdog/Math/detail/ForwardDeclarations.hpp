//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MATH_DETAIL_FORWARDDECLARATIONS_HPP
#define POMDOG_MATH_DETAIL_FORWARDDECLARATIONS_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

// Forward declarations of Math

namespace Pomdog {
namespace Details {

template <typename T>
class FloatingPointCoordinate2D;

template <typename T>
class FloatingPointCoordinate3D;

template <typename RealType>
class FloatingPointColor;

template <typename RealType>
class FloatingPointMatrix2x2;

template <typename RealType>
class FloatingPointMatrix3x3;

template <typename RealType>
class FloatingPointMatrix4x4;

template <typename RealType>
class FloatingPointQuaternion;

template <typename RealType>
class FloatingPointVector2;

template <typename RealType>
class FloatingPointVector3;

template <typename RealType>
class FloatingPointVector4;

template <typename T, class PhantomType>
class TaggedArithmetic;

}// namespace Details

using Color = Details::FloatingPointColor<float>;

}// namespace Pomdog

#endif // !defined(POMDOG_MATH_DETAIL_FORWARDDECLARATIONS_HPP)
