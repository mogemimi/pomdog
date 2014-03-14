//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MATH_DETAIL_FORWARDDECLARATIONS_2B34361D_7375_4924_99CD_35FB0E09144C_HPP
#define POMDOG_MATH_DETAIL_FORWARDDECLARATIONS_2B34361D_7375_4924_99CD_35FB0E09144C_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Config/Export.hpp>
#include <Pomdog/Config/FundamentalTypes.hpp>

// Forward declarations of Math

namespace Pomdog {
namespace Details {

template <typename T>
class Coordinate2D;

template <typename T>
class Coordinate3D;

template <typename RealType>
class POMDOG_EXPORT FloatingPointColor;

template <typename RealType>
class POMDOG_EXPORT FloatingPointMatrix2x2;

template <typename RealType>
class POMDOG_EXPORT FloatingPointMatrix3x3;

template <typename RealType>
class POMDOG_EXPORT FloatingPointMatrix4x4;

template <typename RealType>
class POMDOG_EXPORT FloatingPointQuaternion;

template <typename RealType>
class POMDOG_EXPORT FloatingPointVector2;

template <typename RealType>
class POMDOG_EXPORT FloatingPointVector3;

template <typename RealType>
class POMDOG_EXPORT FloatingPointVector4;

template <typename T, class PhantomType>
class TaggedArithmetic;

namespace Tags {

struct RadianTag;

}// namespace Tags

}// namespace Details

using Color = Details::FloatingPointColor<float>;
using Matrix2x2 = Details::FloatingPointMatrix2x2<float>;
using Matrix3x3 = Details::FloatingPointMatrix3x3<float>;
using Matrix4x4 = Details::FloatingPointMatrix4x4<float>;
using Point2D = Details::Coordinate2D<std::int32_t>;
using Point3D = Details::Coordinate3D<std::int32_t>;
using Quaternion = Details::FloatingPointQuaternion<float>;
using Vector2 = Details::FloatingPointVector2<float>;
using Vector3 = Details::FloatingPointVector3<float>;
using Vector4 = Details::FloatingPointVector4<float>;

template <typename T>
using Radian = Details::TaggedArithmetic<T, Details::Tags::RadianTag>;

// Structures:
class Rectangle;

// Enumerations:
enum class ContainmentType: std::uint16_t;

}// namespace Pomdog

#endif // !defined(POMDOG_MATH_DETAIL_FORWARDDECLARATIONS_2B34361D_7375_4924_99CD_35FB0E09144C_HPP)
