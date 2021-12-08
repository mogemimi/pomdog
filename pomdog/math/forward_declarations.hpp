// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

// Forward declarations of Math

namespace pomdog::detail {

template <typename T>
class Coordinate2D;

template <typename T>
class Coordinate3D;

template <typename RealType>
class POMDOG_EXPORT FloatingPointMatrix2x2;

template <typename RealType>
class POMDOG_EXPORT FloatingPointMatrix3x2;

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

struct DegreeTag;
struct RadianTag;

} // namespace Tags
} // namespace pomdog::detail

namespace pomdog {

using Matrix2x2 = detail::FloatingPointMatrix2x2<float>;
using Matrix3x2 = detail::FloatingPointMatrix3x2<float>;
using Matrix3x3 = detail::FloatingPointMatrix3x3<float>;
using Matrix4x4 = detail::FloatingPointMatrix4x4<float>;
using Point2D = detail::Coordinate2D<std::int32_t>;
using Point3D = detail::Coordinate3D<std::int32_t>;
using Quaternion = detail::FloatingPointQuaternion<float>;
using Vector2 = detail::FloatingPointVector2<float>;
using Vector3 = detail::FloatingPointVector3<float>;
using Vector4 = detail::FloatingPointVector4<float>;

template <typename T>
using Degree = detail::TaggedArithmetic<T, detail::Tags::DegreeTag>;

template <typename T>
using Radian = detail::TaggedArithmetic<T, detail::Tags::RadianTag>;

class BoundingBox;
class BoundingBox2D;
class BoundingCircle;
class BoundingFrustum;
class BoundingSphere;
class Color;
class Plane;
class Ray;
class Rectangle;

// Enumerations:
enum class ContainmentType : std::uint8_t;
enum class PlaneIntersectionType : std::uint8_t;

} // namespace pomdog
