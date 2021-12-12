// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

// Forward declarations of Math

namespace pomdog::detail {

template <typename T, class PhantomType>
class TaggedArithmetic;

namespace Tags {

struct DegreeTag;
struct RadianTag;

} // namespace Tags
} // namespace pomdog::detail

namespace pomdog {

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
class Matrix2x2;
class Matrix3x2;
class Matrix3x3;
class Matrix4x4;
class Plane;
class Point2D;
class Point3D;
class Quaternion;
class Ray;
class Rectangle;
class Vector2;
class Vector3;
class Vector4;

// Enumerations:
enum class ContainmentType : std::uint8_t;
enum class PlaneIntersectionType : std::uint8_t;

} // namespace pomdog
