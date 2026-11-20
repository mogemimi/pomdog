// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/bounding_frustum.h"
#include "pomdog/math/bounding_box.h"
#include "pomdog/math/bounding_sphere.h"
#include "pomdog/math/containment_type.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/plane.h"
#include "pomdog/math/plane_intersection_type.h"
#include "pomdog/math/ray.h"
#include "pomdog/math/simd/f32x4.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <bit>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

namespace simd = math::simd;
using simd::F32x4;

using Planes = std::array<Plane, BoundingFrustum::PlaneCount>;
using Corners = std::array<Vector3, BoundingFrustum::CornerCount>;

/// PlaneLanes is the type of BoundingFrustum::planeLanes_.
using PlaneLanes = f32[2][4][4];

// NOTE: Without SIMD, the scalar versions measured faster: testing the planes
// one by one returns at the first plane that rejects (1.1 to 1.3 times faster
// than testing four planes at once), and computing the corners with Vector3
// was 1.03 times faster than with F32x4Scalar. Both versions are compiled on
// every platform.
//
// NOTE: The helpers that only one of the two versions calls are marked
// [[maybe_unused]], since the other version leaves them unreferenced.

namespace PlaneIndex {
constexpr i32 Near = 0;
constexpr i32 Far = 1;
constexpr i32 Left = 2;
constexpr i32 Right = 3;
constexpr i32 Top = 4;
constexpr i32 Bottom = 5;
} // namespace PlaneIndex

namespace CornerIndex {
constexpr i32 NearLeftTop = 0;
constexpr i32 NearRightTop = 1;
constexpr i32 NearRightBottom = 2;
constexpr i32 NearLeftBottom = 3;
constexpr i32 FarLeftTop = 4;
constexpr i32 FarRightTop = 5;
constexpr i32 FarRightBottom = 6;
constexpr i32 FarLeftBottom = 7;
} // namespace CornerIndex

constexpr auto NLT = CornerIndex::NearLeftTop;
constexpr auto NRT = CornerIndex::NearRightTop;
constexpr auto NRB = CornerIndex::NearRightBottom;
constexpr auto NLB = CornerIndex::NearLeftBottom;
constexpr auto FLT = CornerIndex::FarLeftTop;
constexpr auto FRT = CornerIndex::FarRightTop;
constexpr auto FRB = CornerIndex::FarRightBottom;
constexpr auto FLB = CornerIndex::FarLeftBottom;

constexpr bool CreatePlanesBeforeCorners = true;
constexpr bool FastContains = true;

void makePlane(Plane& plane, f32 x, f32 y, f32 z, f32 d)
{
    plane.normal.x = x;
    plane.normal.y = y;
    plane.normal.z = z;
    plane.distance = d;
}

[[maybe_unused]] [[nodiscard]] Vector3
computeIntersectionPoint(const Plane& a, const Plane& b, const Plane& c)
{
    const auto cross = math::cross(b.normal, c.normal);
    const auto denom = -1.0f * math::dot(a.normal, cross);
    const auto v1 = a.distance * math::cross(b.normal, c.normal);
    const auto v2 = b.distance * math::cross(c.normal, a.normal);
    const auto v3 = c.distance * math::cross(a.normal, b.normal);

    Vector3 result = (v1 + v2 + v3) / denom;
    return result;
}

// NOTE: Lanes lets std::bit_cast move the four members of Plane into F32x4
// without reading through a pointer to normal.x.
struct Lanes final {
    f32 values[4];
};

[[nodiscard]] F32x4
loadPlane(const Plane& plane) noexcept
{
    return simd::load<F32x4>(std::bit_cast<Lanes>(plane).values);
}

/// Returns cross(u, v) of the normals in lanes 0 to 2, computed as math::cross
/// computes it.
[[maybe_unused]] [[nodiscard]] F32x4
crossNormals(F32x4 u, F32x4 v) noexcept
{
    return simd::subtract(
        simd::multiply(simd::swizzle<1, 2, 0, 3>(u), simd::swizzle<2, 0, 1, 3>(v)),
        simd::multiply(simd::swizzle<2, 0, 1, 3>(u), simd::swizzle<1, 2, 0, 3>(v)));
}

/// Returns computeIntersectionPoint(a, b, c) for the planes a, b, and c, given
/// bc = cross(b, c), ca = cross(c, a), and ab = cross(a, b) of their normals.
[[maybe_unused]] [[nodiscard]] Vector3
computeIntersectionPoint(F32x4 a, F32x4 b, F32x4 c, F32x4 bc, F32x4 ca, F32x4 ab) noexcept
{
    // NOTE: (a.x * bc.x + a.y * bc.y) + a.z * bc.z, the order of math::dot.
    const F32x4 m = simd::multiply(a, bc);
    const f32 denom = -1.0f * simd::firstLane(simd::add(simd::add(m, simd::swizzle<1, 1, 1, 1>(m)), simd::swizzle<2, 2, 2, 2>(m)));
    POMDOG_ASSERT(std::fpclassify(denom) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(denom) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(denom) != FP_INFINITE);

    // NOTE: Lane 3 of each plane holds its distance.
    F32x4 sum = simd::multiply(simd::swizzle<3, 3, 3, 3>(a), bc);
    sum = simd::add(sum, simd::multiply(simd::swizzle<3, 3, 3, 3>(b), ca));
    sum = simd::add(sum, simd::multiply(simd::swizzle<3, 3, 3, 3>(c), ab));

    Lanes result;
    simd::store(result.values, simd::divide(sum, simd::splat<F32x4>(denom)));
    return Vector3{result.values[0], result.values[1], result.values[2]};
}

[[maybe_unused]]
void createPlanes(
    std::array<Plane, BoundingFrustum::PlaneCount>& planes,
    const Matrix4x4& matrix)
{
    makePlane(
        planes[PlaneIndex::Near],
        -matrix(0, 2),
        -matrix(1, 2),
        -matrix(2, 2),
        -matrix(3, 2));

    makePlane(
        planes[PlaneIndex::Far],
        -matrix(0, 3) + matrix(0, 2),
        -matrix(1, 3) + matrix(1, 2),
        -matrix(2, 3) + matrix(2, 2),
        -matrix(3, 3) + matrix(3, 2));

    makePlane(
        planes[PlaneIndex::Left],
        -matrix(0, 3) - matrix(0, 0),
        -matrix(1, 3) - matrix(1, 0),
        -matrix(2, 3) - matrix(2, 0),
        -matrix(3, 3) - matrix(3, 0));

    makePlane(
        planes[PlaneIndex::Right],
        -matrix(0, 3) + matrix(0, 0),
        -matrix(1, 3) + matrix(1, 0),
        -matrix(2, 3) + matrix(2, 0),
        -matrix(3, 3) + matrix(3, 0));

    makePlane(
        planes[PlaneIndex::Top],
        -matrix(0, 3) + matrix(0, 1),
        -matrix(1, 3) + matrix(1, 1),
        -matrix(2, 3) + matrix(2, 1),
        -matrix(3, 3) + matrix(3, 1));

    makePlane(
        planes[PlaneIndex::Bottom],
        -matrix(0, 3) - matrix(0, 1),
        -matrix(1, 3) - matrix(1, 1),
        -matrix(2, 3) - matrix(2, 1),
        -matrix(3, 3) - matrix(3, 1));

    for (auto& plane : planes) {
        plane.normalize();
    }
}

[[maybe_unused]]
void createPlanes(
    std::array<Plane, BoundingFrustum::PlaneCount>& planes,
    const std::array<Vector3, BoundingFrustum::CornerCount>& corners)
{
    // NOTE: Left-handed coordinate system
    //
    //   NLT
    //    │\.
    //    │  \.
    //    │  ───────► Normal
    //    │      \.
    //    └─────────.
    //   NLB        NRB
    //
    // Normal == Normalize(Cross(NLT - NLB, NRB - NLT))

    planes[PlaneIndex::Near] = Plane{corners[NLB], corners[NLT], corners[NRB]};
    planes[PlaneIndex::Far] = Plane{corners[FRB], corners[FRT], corners[FLB]};
    planes[PlaneIndex::Left] = Plane{corners[FLB], corners[FLT], corners[NLB]};
    planes[PlaneIndex::Right] = Plane{corners[NRB], corners[NRT], corners[FRB]};
    planes[PlaneIndex::Top] = Plane{corners[NLT], corners[FLT], corners[NRT]};
    planes[PlaneIndex::Bottom] = Plane{corners[FLB], corners[NLB], corners[FRB]};
}

[[maybe_unused]]
void createCorners(
    std::array<Vector3, BoundingFrustum::CornerCount>& corners,
    const Matrix4x4& matrix)
{
    constexpr auto xMin = -1.0f;
    constexpr auto xMax = +1.0f;
    constexpr auto yMin = -1.0f;
    constexpr auto yMax = +1.0f;
    constexpr auto zMin = -1.0f;
    constexpr auto zMax = +1.0f;

    // NOTE: Near clip
    corners[NLT] = Vector3{xMin, yMax, zMin};
    corners[NRT] = Vector3{xMax, yMax, zMin};
    corners[NRB] = Vector3{xMax, yMin, zMin};
    corners[NLB] = Vector3{xMin, yMin, zMin};

    // NOTE: Far clip
    corners[FLT] = Vector3{xMin, yMax, zMax};
    corners[FRT] = Vector3{xMax, yMax, zMax};
    corners[FRB] = Vector3{xMax, yMin, zMax};
    corners[FLB] = Vector3{xMin, yMin, zMax};

    auto inverseMatrix = math::invert(matrix);
    for (auto& corner : corners) {
        corner = math::transform(corner, inverseMatrix);
    }
}

void transposePlanes(
    PlaneLanes& lanes,
    const std::array<Plane, BoundingFrustum::PlaneCount>& planes) noexcept
{
    // NOTE: Lanes 6 and 7 repeat planes 0 and 1, which changes no answer to
    // "does any plane reject" or "do all planes accept".
    simd::storeTransposed(lanes[0], loadPlane(planes[0]), loadPlane(planes[1]), loadPlane(planes[2]), loadPlane(planes[3]));
    simd::storeTransposed(lanes[1], loadPlane(planes[4]), loadPlane(planes[5]), loadPlane(planes[0]), loadPlane(planes[1]));
}

void createPlaneLanes(PlaneLanes& lanes, const Matrix4x4& matrix)
{
    Planes planes;
    if constexpr (CreatePlanesBeforeCorners) {
        createPlanes(planes, matrix);
    }
    else {
        Corners corners;
        createCorners(corners, matrix);
        createPlanes(planes, corners);
    }
    transposePlanes(lanes, planes);
}

/// Returns plane index of lanes.
[[nodiscard]] Plane
getPlane(const PlaneLanes& lanes, i32 index) noexcept
{
    POMDOG_ASSERT(index >= 0 && index < BoundingFrustum::PlaneCount);
    const auto& group = lanes[index / 4];
    const auto lane = index % 4;
    return Plane{Vector3{group[0][lane], group[1][lane], group[2][lane]}, group[3][lane]};
}

/// Returns the six planes of lanes, in the order of PlaneIndex.
[[nodiscard]] Planes
getPlanes(const PlaneLanes& lanes) noexcept
{
    return Planes{
        getPlane(lanes, 0),
        getPlane(lanes, 1),
        getPlane(lanes, 2),
        getPlane(lanes, 3),
        getPlane(lanes, 4),
        getPlane(lanes, 5),
    };
}

[[nodiscard]] Corners
computeCorners(const PlaneLanes& lanes) noexcept
{
    constexpr auto N = PlaneIndex::Near;
    constexpr auto F = PlaneIndex::Far;
    constexpr auto L = PlaneIndex::Left;
    constexpr auto R = PlaneIndex::Right;
    constexpr auto T = PlaneIndex::Top;
    constexpr auto B = PlaneIndex::Bottom;

    Corners corners;
    if constexpr (simd::HasSIMD) {
        // NOTE: Transposes the lanes back into one plane per row, so that
        // rows[i / 4][i % 4] holds plane i.
        f32 rows[2][4][4];
        for (i32 group = 0; group < 2; ++group) {
            simd::storeTransposed(
                rows[group],
                simd::load<F32x4>(lanes[group][0]),
                simd::load<F32x4>(lanes[group][1]),
                simd::load<F32x4>(lanes[group][2]),
                simd::load<F32x4>(lanes[group][3]));
        }

        // NOTE: The eight corners use only twelve different cross products,
        // and each corner gets the same values that the scalar version computes.
        const F32x4 n = simd::load<F32x4>(rows[N / 4][N % 4]);
        const F32x4 f = simd::load<F32x4>(rows[F / 4][F % 4]);
        const F32x4 l = simd::load<F32x4>(rows[L / 4][L % 4]);
        const F32x4 r = simd::load<F32x4>(rows[R / 4][R % 4]);
        const F32x4 t = simd::load<F32x4>(rows[T / 4][T % 4]);
        const F32x4 b = simd::load<F32x4>(rows[B / 4][B % 4]);
        const F32x4 lt = crossNormals(l, t);
        const F32x4 rt = crossNormals(r, t);
        const F32x4 rb = crossNormals(r, b);
        const F32x4 lb = crossNormals(l, b);
        const F32x4 tn = crossNormals(t, n);
        const F32x4 bn = crossNormals(b, n);
        const F32x4 tf = crossNormals(t, f);
        const F32x4 bf = crossNormals(b, f);
        const F32x4 nl = crossNormals(n, l);
        const F32x4 nr = crossNormals(n, r);
        const F32x4 fl = crossNormals(f, l);
        const F32x4 fr = crossNormals(f, r);
        corners[NLT] = computeIntersectionPoint(n, l, t, lt, tn, nl);
        corners[NRT] = computeIntersectionPoint(n, r, t, rt, tn, nr);
        corners[NRB] = computeIntersectionPoint(n, r, b, rb, bn, nr);
        corners[NLB] = computeIntersectionPoint(n, l, b, lb, bn, nl);
        corners[FLT] = computeIntersectionPoint(f, l, t, lt, tf, fl);
        corners[FRT] = computeIntersectionPoint(f, r, t, rt, tf, fr);
        corners[FRB] = computeIntersectionPoint(f, r, b, rb, bf, fr);
        corners[FLB] = computeIntersectionPoint(f, l, b, lb, bf, fl);
    }
    else {
        const Planes planes = getPlanes(lanes);
        corners[NLT] = computeIntersectionPoint(planes[N], planes[L], planes[T]);
        corners[NRT] = computeIntersectionPoint(planes[N], planes[R], planes[T]);
        corners[NRB] = computeIntersectionPoint(planes[N], planes[R], planes[B]);
        corners[NLB] = computeIntersectionPoint(planes[N], planes[L], planes[B]);
        corners[FLT] = computeIntersectionPoint(planes[F], planes[L], planes[T]);
        corners[FRT] = computeIntersectionPoint(planes[F], planes[R], planes[T]);
        corners[FRB] = computeIntersectionPoint(planes[F], planes[R], planes[B]);
        corners[FLB] = computeIntersectionPoint(planes[F], planes[L], planes[B]);
    }
    return corners;
}

/// Returns Front or Back when all corners lie on that side of plane, and
/// Intersecting otherwise.
[[nodiscard]] PlaneIntersectionType
intersectsCorners(const Plane& plane, const Corners& corners) noexcept
{
    auto result = plane.intersects(corners.front());
    for (u32 i = 1; i < corners.size(); i++) {
        if (result != plane.intersects(corners[i])) {
            result = PlaneIntersectionType::Intersecting;
        }
    }
    return result;
}

/// Returns the largest of the four lanes of v.
[[nodiscard]] f32
largestLane(F32x4 v) noexcept
{
    v = simd::max(v, simd::swizzle<1, 0, 3, 2>(v));
    v = simd::max(v, simd::swizzle<2, 3, 0, 1>(v));
    return simd::firstLane(v);
}

/// Returns n . p + d of the four planes of group for the point (x, y, z) in
/// every lane, summed in the order of Plane::dotCoordinate.
[[nodiscard]] F32x4
dotCoordinates(const f32 (&group)[4][4], F32x4 x, F32x4 y, F32x4 z) noexcept
{
    F32x4 r = simd::multiply(simd::load<F32x4>(group[0]), x);
    r = simd::add(r, simd::multiply(simd::load<F32x4>(group[1]), y));
    r = simd::add(r, simd::multiply(simd::load<F32x4>(group[2]), z));
    return simd::add(r, simd::load<F32x4>(group[3]));
}

/// BoxLanes holds the minimum and maximum of a box, each coordinate in every lane.
struct BoxLanes final {
    F32x4 minX;
    F32x4 minY;
    F32x4 minZ;
    F32x4 maxX;
    F32x4 maxY;
    F32x4 maxZ;
};

[[maybe_unused]] [[nodiscard]] BoxLanes
splatBox(const BoundingBox& box) noexcept
{
    return BoxLanes{
        .minX = simd::splat<F32x4>(box.min.x),
        .minY = simd::splat<F32x4>(box.min.y),
        .minZ = simd::splat<F32x4>(box.min.z),
        .maxX = simd::splat<F32x4>(box.max.x),
        .maxY = simd::splat<F32x4>(box.max.y),
        .maxZ = simd::splat<F32x4>(box.max.z),
    };
}

// NOTE: Plane::intersects(BoundingBox) picks, for each axis, the box coordinate
// that makes n * v smallest (the negative vertex) or largest (the positive
// vertex). For a box with min <= max, that product is the smaller or larger of
// n * min and n * max, so these functions get the same sums without choosing a
// vertex for each plane.

/// Returns n . v + d of the four planes of group for their negative vertices.
[[maybe_unused]] [[nodiscard]] F32x4
negativeVertexDots(const f32 (&group)[4][4], const BoxLanes& box) noexcept
{
    const F32x4 nx = simd::load<F32x4>(group[0]);
    const F32x4 ny = simd::load<F32x4>(group[1]);
    const F32x4 nz = simd::load<F32x4>(group[2]);
    F32x4 r = simd::min(simd::multiply(nx, box.minX), simd::multiply(nx, box.maxX));
    r = simd::add(r, simd::min(simd::multiply(ny, box.minY), simd::multiply(ny, box.maxY)));
    r = simd::add(r, simd::min(simd::multiply(nz, box.minZ), simd::multiply(nz, box.maxZ)));
    return simd::add(r, simd::load<F32x4>(group[3]));
}

/// Returns n . v + d of the four planes of group for their positive vertices.
[[maybe_unused]] [[nodiscard]] F32x4
positiveVertexDots(const f32 (&group)[4][4], const BoxLanes& box) noexcept
{
    const F32x4 nx = simd::load<F32x4>(group[0]);
    const F32x4 ny = simd::load<F32x4>(group[1]);
    const F32x4 nz = simd::load<F32x4>(group[2]);
    F32x4 r = simd::max(simd::multiply(nx, box.minX), simd::multiply(nx, box.maxX));
    r = simd::add(r, simd::max(simd::multiply(ny, box.minY), simd::multiply(ny, box.maxY)));
    r = simd::add(r, simd::max(simd::multiply(nz, box.minZ), simd::multiply(nz, box.maxZ)));
    return simd::add(r, simd::load<F32x4>(group[3]));
}

/// Returns the largest n . p + d of the six planes for point.
[[maybe_unused]] [[nodiscard]] f32
largestDotCoordinate(const PlaneLanes& lanes, const Vector3& point) noexcept
{
    const F32x4 x = simd::splat<F32x4>(point.x);
    const F32x4 y = simd::splat<F32x4>(point.y);
    const F32x4 z = simd::splat<F32x4>(point.z);
    return largestLane(simd::max(dotCoordinates(lanes[0], x, y, z), dotCoordinates(lanes[1], x, y, z)));
}

} // namespace

BoundingFrustum::BoundingFrustum() = default;

BoundingFrustum::BoundingFrustum(const Matrix4x4& matrix)
{
    createPlaneLanes(planeLanes_, matrix);
}

void BoundingFrustum::setMatrix(const Matrix4x4& matrix) noexcept
{
    createPlaneLanes(planeLanes_, matrix);
}

Plane BoundingFrustum::getNear() const noexcept
{
    return getPlane(planeLanes_, PlaneIndex::Near);
}

Plane BoundingFrustum::getFar() const noexcept
{
    return getPlane(planeLanes_, PlaneIndex::Far);
}

Plane BoundingFrustum::getLeft() const noexcept
{
    return getPlane(planeLanes_, PlaneIndex::Left);
}

Plane BoundingFrustum::getRight() const noexcept
{
    return getPlane(planeLanes_, PlaneIndex::Right);
}

Plane BoundingFrustum::getTop() const noexcept
{
    return getPlane(planeLanes_, PlaneIndex::Top);
}

Plane BoundingFrustum::getBottom() const noexcept
{
    return getPlane(planeLanes_, PlaneIndex::Bottom);
}

std::array<Vector3, BoundingFrustum::CornerCount>
BoundingFrustum::getCorners() const noexcept
{
    return computeCorners(planeLanes_);
}

ContainmentType
BoundingFrustum::contains(const Vector3& point) const noexcept
{
    if constexpr (FastContains && simd::HasSIMD) {
        if (largestDotCoordinate(planeLanes_, point) > 0.0f) {
            return ContainmentType::Disjoint;
        }
        return ContainmentType::Contains;
    }
    else if constexpr (FastContains) {
        // NOTE: fast mode
        for (const auto& plane : getPlanes(planeLanes_)) {
            const auto distance = plane.dotCoordinate(point);
            if (distance > 0.0f) {
                return ContainmentType::Disjoint;
            }
        }
        return ContainmentType::Contains;
    }
    else {
        bool intersects = false;
        for (const auto& plane : getPlanes(planeLanes_)) {
            const auto distance = plane.dotCoordinate(point);
            if (distance > 0.0f) {
                return ContainmentType::Disjoint;
            }
            if (distance == 0.0f) {
                intersects = true;
            }
        }
        if (intersects) {
            return ContainmentType::Intersects;
        }
        return ContainmentType::Contains;
    }
}

ContainmentType
BoundingFrustum::contains(const BoundingBox& box) const noexcept
{
    if constexpr (simd::HasSIMD) {
        // NOTE: A plane has the box in front when its negative vertex is in
        // front, and behind when its positive vertex is behind.
        const BoxLanes lanes = splatBox(box);
        if (largestLane(simd::max(negativeVertexDots(planeLanes_[0], lanes), negativeVertexDots(planeLanes_[1], lanes))) > 0.0f) {
            return ContainmentType::Disjoint;
        }
        if (largestLane(simd::max(positiveVertexDots(planeLanes_[0], lanes), positiveVertexDots(planeLanes_[1], lanes))) < 0.0f) {
            return ContainmentType::Contains;
        }
        return ContainmentType::Intersects;
    }
    else {
        bool intersects = false;
        for (const auto& plane : getPlanes(planeLanes_)) {
            const auto planeIntersectionType = plane.intersects(box);
            if (planeIntersectionType == PlaneIntersectionType::Front) {
                return ContainmentType::Disjoint;
            }
            if (planeIntersectionType == PlaneIntersectionType::Intersecting) {
                intersects = true;
            }
        }
        if (intersects) {
            return ContainmentType::Intersects;
        }
        return ContainmentType::Contains;
    }
}

ContainmentType
BoundingFrustum::contains(const BoundingFrustum& frustum) const noexcept
{
    // NOTE: Computes the corners of frustum once for the six planes.
    const auto corners = frustum.getCorners();
    bool intersects = false;
    for (const auto& plane : getPlanes(planeLanes_)) {
        const auto planeIntersectionType = intersectsCorners(plane, corners);
        if (planeIntersectionType == PlaneIntersectionType::Front) {
            return ContainmentType::Disjoint;
        }
        if (planeIntersectionType == PlaneIntersectionType::Intersecting) {
            intersects = true;
        }
    }
    if (intersects) {
        return ContainmentType::Intersects;
    }
    return ContainmentType::Contains;
}

ContainmentType
BoundingFrustum::contains(const BoundingSphere& sphere) const noexcept
{
    if constexpr (simd::HasSIMD) {
        const f32 distance = largestDotCoordinate(planeLanes_, sphere.center);
        if (distance > sphere.radius) {
            return ContainmentType::Disjoint;
        }
        if (distance < -sphere.radius) {
            return ContainmentType::Contains;
        }
        return ContainmentType::Intersects;
    }
    else {
        bool intersects = false;
        for (const auto& plane : getPlanes(planeLanes_)) {
            const auto planeIntersectionType = plane.intersects(sphere);
            if (planeIntersectionType == PlaneIntersectionType::Front) {
                return ContainmentType::Disjoint;
            }
            if (planeIntersectionType == PlaneIntersectionType::Intersecting) {
                intersects = true;
            }
        }
        if (intersects) {
            return ContainmentType::Intersects;
        }
        return ContainmentType::Contains;
    }
}

bool BoundingFrustum::intersects(const BoundingBox& box) const noexcept
{
    if constexpr (simd::HasSIMD) {
        // NOTE: Testing the second group only when the first one accepts the
        // box measured faster than testing both groups at once.
        const BoxLanes lanes = splatBox(box);
        if (largestLane(negativeVertexDots(planeLanes_[0], lanes)) > 0.0f) {
            return false;
        }
        return !(largestLane(negativeVertexDots(planeLanes_[1], lanes)) > 0.0f);
    }
    else {
        for (const auto& plane : getPlanes(planeLanes_)) {
            if (plane.intersects(box) == PlaneIntersectionType::Front) {
                return false;
            }
        }
        return true;
    }
}

bool BoundingFrustum::intersects(const BoundingFrustum& frustum) const noexcept
{
    // NOTE: Computes the corners of frustum once for the six planes.
    const auto corners = frustum.getCorners();
    for (const auto& plane : getPlanes(planeLanes_)) {
        const auto planeIntersectionType = intersectsCorners(plane, corners);
        if (planeIntersectionType == PlaneIntersectionType::Front) {
            return false;
        }
    }
    return true;
}

bool BoundingFrustum::intersects(const BoundingSphere& sphere) const noexcept
{
    if constexpr (simd::HasSIMD) {
        return !(largestDotCoordinate(planeLanes_, sphere.center) > sphere.radius);
    }
    else {
        for (const auto& plane : getPlanes(planeLanes_)) {
            const auto planeIntersectionType = plane.intersects(sphere);
            if (planeIntersectionType == PlaneIntersectionType::Front) {
                return false;
            }
        }
        return true;
    }
}

PlaneIntersectionType
BoundingFrustum::intersects(const Plane& plane) const noexcept
{
    return intersectsCorners(plane, getCorners());
}

std::optional<f32>
BoundingFrustum::intersects(const Ray& ray) const noexcept
{
    const auto planes = getPlanes(planeLanes_);
    std::array<std::optional<f32>, PlaneCount> distances;
    for (i32 i = 0; i < PlaneCount; i++) {
        const auto& plane = planes[i];
        const auto d = ray.intersects(plane);
        const auto distanceToRayPosition = plane.dotCoordinate(ray.position);
        if ((distanceToRayPosition < 0.0f) && !d) {
            return std::nullopt;
        }
        distances[i] = d;
        POMDOG_ASSERT(!distances[i] || *distances[i] >= 0.0f);
    }

    auto d = distances.front();
    POMDOG_ASSERT(!d || *d >= 0.0f);
    for (i32 i = 1; i < PlaneCount; i++) {
        if (!d) {
            d = distances[i];
            POMDOG_ASSERT(!d || *d >= 0.0f);
            continue;
        }
        if (distances[i]) {
            POMDOG_ASSERT(d);
            d = std::min(*distances[i], *d);
            POMDOG_ASSERT(d && *d >= 0.0f);
        }
    }
    POMDOG_ASSERT(!d || *d >= 0.0f);
    return d;
}

} // namespace pomdog
