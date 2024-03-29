// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/matrix4x4.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/math.h"
#include "pomdog/math/matrix3x3.h"
#include "pomdog/math/quaternion.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector3.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Matrix4x4::Matrix4x4() noexcept = default;

Matrix4x4::Matrix4x4(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33) noexcept
{
    m[0][0] = m00;
    m[0][1] = m01;
    m[0][2] = m02;
    m[0][3] = m03;
    m[1][0] = m10;
    m[1][1] = m11;
    m[1][2] = m12;
    m[1][3] = m13;
    m[2][0] = m20;
    m[2][1] = m21;
    m[2][2] = m22;
    m[2][3] = m23;
    m[3][0] = m30;
    m[3][1] = m31;
    m[3][2] = m32;
    m[3][3] = m33;
}

const float& Matrix4x4::operator()(std::size_t row, std::size_t column) const noexcept
{
    static_assert(std::is_same_v<decltype(m), float[4][4]>);
    POMDOG_ASSERT_MESSAGE(row < 4, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < 4, "column: out of range");
    return m[row][column];
}

float& Matrix4x4::operator()(std::size_t row, std::size_t column) noexcept
{
    static_assert(std::is_same_v<decltype(m), float[4][4]>);
    POMDOG_ASSERT_MESSAGE(row < 4, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < 4, "column: out of range");
    return m[row][column];
}

Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& other) noexcept
{
    *this = math::multiply(*this, other);
    return *this;
}

Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& other) noexcept
{
    m[0][0] += other.m[0][0];
    m[0][1] += other.m[0][1];
    m[0][2] += other.m[0][2];
    m[0][3] += other.m[0][3];
    m[1][0] += other.m[1][0];
    m[1][1] += other.m[1][1];
    m[1][2] += other.m[1][2];
    m[1][3] += other.m[1][3];
    m[2][0] += other.m[2][0];
    m[2][1] += other.m[2][1];
    m[2][2] += other.m[2][2];
    m[2][3] += other.m[2][3];
    m[3][0] += other.m[3][0];
    m[3][1] += other.m[3][1];
    m[3][2] += other.m[3][2];
    m[3][3] += other.m[3][3];
    return *this;
}

Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& other) noexcept
{
    m[0][0] -= other.m[0][0];
    m[0][1] -= other.m[0][1];
    m[0][2] -= other.m[0][2];
    m[0][3] -= other.m[0][3];
    m[1][0] -= other.m[1][0];
    m[1][1] -= other.m[1][1];
    m[1][2] -= other.m[1][2];
    m[1][3] -= other.m[1][3];
    m[2][0] -= other.m[2][0];
    m[2][1] -= other.m[2][1];
    m[2][2] -= other.m[2][2];
    m[2][3] -= other.m[2][3];
    m[3][0] -= other.m[3][0];
    m[3][1] -= other.m[3][1];
    m[3][2] -= other.m[3][2];
    m[3][3] -= other.m[3][3];
    return *this;
}

Matrix4x4& Matrix4x4::operator*=(float factor) noexcept
{
    *this = math::multiply(*this, factor);
    return *this;
}

Matrix4x4& Matrix4x4::operator/=(float factor) noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    const auto inverseDivider = 1.0f / factor;
    m[0][0] *= inverseDivider;
    m[0][1] *= inverseDivider;
    m[0][2] *= inverseDivider;
    m[0][3] *= inverseDivider;
    m[1][0] *= inverseDivider;
    m[1][1] *= inverseDivider;
    m[1][2] *= inverseDivider;
    m[1][3] *= inverseDivider;
    m[2][0] *= inverseDivider;
    m[2][1] *= inverseDivider;
    m[2][2] *= inverseDivider;
    m[2][3] *= inverseDivider;
    m[3][0] *= inverseDivider;
    m[3][1] *= inverseDivider;
    m[3][2] *= inverseDivider;
    m[3][3] *= inverseDivider;
    return *this;
}

Matrix4x4 Matrix4x4::operator+() const noexcept
{
    return *this;
}

Matrix4x4 Matrix4x4::operator-() const noexcept
{
    return math::multiply(*this, -1);
}

Matrix4x4 Matrix4x4::operator+(const Matrix4x4& other) const noexcept
{
    return Matrix4x4{
        m[0][0] + other.m[0][0],
        m[0][1] + other.m[0][1],
        m[0][2] + other.m[0][2],
        m[0][3] + other.m[0][3],
        m[1][0] + other.m[1][0],
        m[1][1] + other.m[1][1],
        m[1][2] + other.m[1][2],
        m[1][3] + other.m[1][3],
        m[2][0] + other.m[2][0],
        m[2][1] + other.m[2][1],
        m[2][2] + other.m[2][2],
        m[2][3] + other.m[2][3],
        m[3][0] + other.m[3][0],
        m[3][1] + other.m[3][1],
        m[3][2] + other.m[3][2],
        m[3][3] + other.m[3][3],
    };
}

Matrix4x4 Matrix4x4::operator-(const Matrix4x4& other) const noexcept
{
    return Matrix4x4{
        m[0][0] - other.m[0][0],
        m[0][1] - other.m[0][1],
        m[0][2] - other.m[0][2],
        m[0][3] - other.m[0][3],
        m[1][0] - other.m[1][0],
        m[1][1] - other.m[1][1],
        m[1][2] - other.m[1][2],
        m[1][3] - other.m[1][3],
        m[2][0] - other.m[2][0],
        m[2][1] - other.m[2][1],
        m[2][2] - other.m[2][2],
        m[2][3] - other.m[2][3],
        m[3][0] - other.m[3][0],
        m[3][1] - other.m[3][1],
        m[3][2] - other.m[3][2],
        m[3][3] - other.m[3][3],
    };
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const noexcept
{
    return math::multiply(*this, other);
}

Matrix4x4 Matrix4x4::operator*(float factor) const noexcept
{
    return math::multiply(*this, factor);
}

Matrix4x4 Matrix4x4::operator/(float factor) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    const auto inverseDivider = 1.0f / factor;
    return Matrix4x4{
        m[0][0] * inverseDivider,
        m[0][1] * inverseDivider,
        m[0][2] * inverseDivider,
        m[0][3] * inverseDivider,
        m[1][0] * inverseDivider,
        m[1][1] * inverseDivider,
        m[1][2] * inverseDivider,
        m[1][3] * inverseDivider,
        m[2][0] * inverseDivider,
        m[2][1] * inverseDivider,
        m[2][2] * inverseDivider,
        m[2][3] * inverseDivider,
        m[3][0] * inverseDivider,
        m[3][1] * inverseDivider,
        m[3][2] * inverseDivider,
        m[3][3] * inverseDivider,
    };
}

bool Matrix4x4::operator==(const Matrix4x4& other) const noexcept
{
    return m[0][0] == other.m[0][0] &&
           m[0][1] == other.m[0][1] &&
           m[0][2] == other.m[0][2] &&
           m[0][3] == other.m[0][3] &&
           m[1][0] == other.m[1][0] &&
           m[1][1] == other.m[1][1] &&
           m[1][2] == other.m[1][2] &&
           m[1][3] == other.m[1][3] &&
           m[2][0] == other.m[2][0] &&
           m[2][1] == other.m[2][1] &&
           m[2][2] == other.m[2][2] &&
           m[2][3] == other.m[2][3] &&
           m[3][0] == other.m[3][0] &&
           m[3][1] == other.m[3][1] &&
           m[3][2] == other.m[3][2] &&
           m[3][3] == other.m[3][3];
}

bool Matrix4x4::operator!=(const Matrix4x4& other) const noexcept
{
    return m[0][0] != other.m[0][0] ||
           m[0][1] != other.m[0][1] ||
           m[0][2] != other.m[0][2] ||
           m[0][3] != other.m[0][3] ||
           m[1][0] != other.m[1][0] ||
           m[1][1] != other.m[1][1] ||
           m[1][2] != other.m[1][2] ||
           m[1][3] != other.m[1][3] ||
           m[2][0] != other.m[2][0] ||
           m[2][1] != other.m[2][1] ||
           m[2][2] != other.m[2][2] ||
           m[2][3] != other.m[2][3] ||
           m[3][0] != other.m[3][0] ||
           m[3][1] != other.m[3][1] ||
           m[3][2] != other.m[3][2] ||
           m[3][3] != other.m[3][3];
}

void Matrix4x4::setTranslation(const Vector3& position) noexcept
{
    m[3][0] = position.x;
    m[3][1] = position.y;
    m[3][2] = position.z;
}

Vector3 Matrix4x4::getTranslation() const noexcept
{
    return Vector3{m[3][0], m[3][1], m[3][2]};
}

void Matrix4x4::setScale(const Vector3& scale) noexcept
{
    m[0][0] = scale.x;
    m[1][1] = scale.y;
    m[2][2] = scale.z;
}

Vector3 Matrix4x4::getScale() const noexcept
{
    return Vector3{m[0][0], m[1][1], m[2][2]};
}

Matrix4x4
Matrix4x4::createTranslation(const Vector3& position) noexcept
{
    return Matrix4x4{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        position.x, position.y, position.z, 1.0f};
}

Matrix4x4
Matrix4x4::createScale(float scale) noexcept
{
    return Matrix4x4{
        scale, 0.0f, 0.0f, 0.0f,
        0.0f, scale, 0.0f, 0.0f,
        0.0f, 0.0f, scale, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
}

Matrix4x4
Matrix4x4::createScale(const Vector3& scale) noexcept
{
    return Matrix4x4{
        scale.x, 0.0f, 0.0f, 0.0f,
        0.0f, scale.y, 0.0f, 0.0f,
        0.0f, 0.0f, scale.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
}

Matrix4x4
Matrix4x4::createRotationX(const Radian<float>& angle)
{
    const auto sinAngle = std::sin(angle.value);
    const auto cosAngle = std::cos(angle.value);

    return Matrix4x4{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, cosAngle, sinAngle, 0.0f,
        0.0f, -sinAngle, cosAngle, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
}

Matrix4x4
Matrix4x4::createRotationY(const Radian<float>& angle)
{
    const auto sinAngle = std::sin(angle.value);
    const auto cosAngle = std::cos(angle.value);

    return Matrix4x4{
        cosAngle, 0.0f, -sinAngle, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        sinAngle, 0.0f, cosAngle, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
}

Matrix4x4
Matrix4x4::createRotationZ(const Radian<float>& angle)
{
    const auto sinAngle = std::sin(angle.value);
    const auto cosAngle = std::cos(angle.value);

    return Matrix4x4{
        cosAngle, sinAngle, 0.0f, 0.0f,
        -sinAngle, cosAngle, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
}

Matrix4x4
Matrix4x4::createFromQuaternion(const Quaternion& quaternion)
{
    const auto xx = quaternion.x * quaternion.x;
    const auto yy = quaternion.y * quaternion.y;
    const auto zz = quaternion.z * quaternion.z;
    const auto xy = quaternion.x * quaternion.y;
    const auto zw = quaternion.z * quaternion.w;
    const auto zx = quaternion.z * quaternion.x;
    const auto yw = quaternion.y * quaternion.w;
    const auto yz = quaternion.y * quaternion.z;
    const auto xw = quaternion.x * quaternion.w;

    return Matrix4x4{
        1.0f - (2.0f * (yy + zz)),
        2.0f * (xy + zw),
        2.0f * (zx - yw),
        0.0f,

        2.0f * (xy - zw),
        1.0f - (2.0f * (zz + xx)),
        2.0f * (yz + xw),
        0.0f,

        2.0f * (zx + yw),
        2.0f * (yz - xw),
        1.0f - (2.0f * (yy + xx)),
        0.0f,

        0.0f,
        0.0f,
        0.0f,
        1.0f,
    };
}

Matrix4x4
Matrix4x4::createLookAtLH(const Vector3& eye, const Vector3& at, const Vector3& up)
{
    // NOTE: Left-handed Cartesian Coordinates
    //
    // zaxis = normal(At - Eye)
    // xaxis = normal(cross(Up, zaxis))
    // yaxis = cross(zaxis, xaxis)
    //
    //  xaxis.x           yaxis.x           zaxis.x          0
    //  xaxis.y           yaxis.y           zaxis.y          0
    //  xaxis.z           yaxis.z           zaxis.z          0
    // -dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  1

    const auto zaxis = math::normalize(at - eye); // RH: eye - at
    const auto xaxis = math::normalize(math::cross(up, zaxis));
    const auto yaxis = math::cross(zaxis, xaxis);

    return Matrix4x4{
        xaxis.x,
        yaxis.x,
        zaxis.x,
        0.0f,

        xaxis.y,
        yaxis.y,
        zaxis.y,
        0.0f,

        xaxis.z,
        yaxis.z,
        zaxis.z,
        0.0f,

        -math::dot(xaxis, eye),
        -math::dot(yaxis, eye),
        -math::dot(zaxis, eye),
        1.0f,
    };
}

Matrix4x4
Matrix4x4::createLookAtRH(const Vector3& eye, const Vector3& at, const Vector3& up)
{
    // NOTE: Right-handed Cartesian Coordinates
    //
    // zaxis = normal(Eye - At)
    // xaxis = normal(cross(Up, zaxis))
    // yaxis = cross(zaxis, xaxis)
    //
    //  xaxis.x           yaxis.x           zaxis.x          0
    //  xaxis.y           yaxis.y           zaxis.y          0
    //  xaxis.z           yaxis.z           zaxis.z          0
    // -dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  1

    const auto zaxis = math::normalize(eye - at); // LH: at - eye
    const auto xaxis = math::normalize(math::cross(up, zaxis));
    const auto yaxis = math::cross(zaxis, xaxis);

    return Matrix4x4{
        xaxis.x,
        yaxis.x,
        zaxis.x,
        0.0f,

        xaxis.y,
        yaxis.y,
        zaxis.y,
        0.0f,

        xaxis.z,
        yaxis.z,
        zaxis.z,
        0.0f,

        -math::dot(xaxis, eye),
        -math::dot(yaxis, eye),
        -math::dot(zaxis, eye),
        1.0f,
    };
}

Matrix4x4
Matrix4x4::createPerspectiveLH(float width, float height, float zNear, float zFar)
{
    POMDOG_ASSERT_MESSAGE(width > 0, "width: out of range");
    POMDOG_ASSERT_MESSAGE(height > 0, "height: out of range");
    POMDOG_ASSERT_MESSAGE(zNear > 0, "zNear: out of range");
    POMDOG_ASSERT_MESSAGE(zFar > 0, "zFar: out of range");
    POMDOG_ASSERT_MESSAGE(zNear < zFar, "zNear: out of range");

    // NOTE: Left-handed Cartesian Coordinates
    //
    // 2*zn/w  0       0              0
    // 0       2*zn/h  0              0
    // 0       0       zf/(zf-zn)     1
    // 0       0       zn*zf/(zn-zf)  0

    const auto a = 2.0f * zNear / width;
    const auto b = 2.0f * zNear / height;
    const auto c = zFar / (zFar - zNear); // RH: c = zFar / (zNear - zFar);
    const auto d = zNear * zFar / (zNear - zFar);

    return Matrix4x4{
        a,
        0.0f,
        0.0f,
        0.0f,

        0.0f,
        b,
        0.0f,
        0.0f,

        0.0f,
        0.0f,
        c,
        1.0f, // RH: -1

        0.0f,
        0.0f,
        d,
        0.0f,
    };
}

Matrix4x4
Matrix4x4::createPerspectiveRH(float width, float height, float zNear, float zFar)
{
    POMDOG_ASSERT_MESSAGE(width > 0, "width: out of range");
    POMDOG_ASSERT_MESSAGE(height > 0, "height: out of range");
    POMDOG_ASSERT_MESSAGE(zNear > 0, "zNear: out of range");
    POMDOG_ASSERT_MESSAGE(zFar > 0, "zFar: out of range");
    POMDOG_ASSERT_MESSAGE(zNear < zFar, "zNear: out of range");

    // NOTE: Right-handed Cartesian Coordinates
    //
    // 2*zn/w  0       0              0
    // 0       2*zn/h  0              0
    // 0       0       zf/(zn-zf)    -1
    // 0       0       zn*zf/(zn-zf)  0

    const auto a = 2.0f * zNear / width;
    const auto b = 2.0f * zNear / height;
    const auto c = zFar / (zNear - zFar); //LH: c = zFar / (zFar - zNear);
    const auto d = zNear * zFar / (zNear - zFar);

    return Matrix4x4{
        a,
        0.0f,
        0.0f,
        0.0f,

        0.0f,
        b,
        0.0f,
        0.0f,

        0.0f,
        0.0f,
        c,
        -1.0f, // LH: +1

        0.0f,
        0.0f,
        d,
        0.0f,
    };
}

Matrix4x4
Matrix4x4::createPerspectiveFieldOfViewLH(const Radian<float>& fovy, float aspect, float zNear, float zFar)
{
    POMDOG_ASSERT_MESSAGE(fovy > 0, "fovy: out of range");
    POMDOG_ASSERT_MESSAGE(aspect > 0, "aspect: out of range");
    POMDOG_ASSERT_MESSAGE(zNear > 0, "zNear: out of range");
    POMDOG_ASSERT_MESSAGE(zFar > 0, "zFar: out of range");
    POMDOG_ASSERT_MESSAGE(zNear < zFar, "zNear: out of range");

    // NOTE: Left-handed Cartesian Coordinates
    //
    // f = cotangent(fovY/2)
    // cotangent(fovY/2) = 1/tan(fovY/2)
    //
    // The generated matrix is:
    // (f/Aspect)  0   0               0
    // 0           f   0               0
    // 0           0   zf/(zf-zn)      1
    // 0           0   -zn*zf/(zf-zn)  0

    const auto yScale = 1.0f / std::tan(fovy.value * 0.5f);
    const auto xScale = yScale / aspect;
    const auto p = zFar / (zFar - zNear);

    return Matrix4x4{
        xScale,
        0.0f,
        0.0f,
        0.0f,

        0.0f,
        yScale,
        0.0f,
        0.0f,

        0.0f,
        0.0f,
        p,
        1.0f,

        0.0f,
        0.0f,
        -zNear * p,
        0.0f,
    };
}

Matrix4x4
Matrix4x4::createPerspectiveFieldOfViewRH(const Radian<float>& fovy, float aspect, float zNear, float zFar)
{
    POMDOG_ASSERT_MESSAGE(fovy > 0, "fovy: out of range");
    POMDOG_ASSERT_MESSAGE(aspect > 0, "aspect: out of range");
    POMDOG_ASSERT_MESSAGE(zNear > 0, "zNear: out of range");
    POMDOG_ASSERT_MESSAGE(zFar > 0, "zFar: out of range");
    POMDOG_ASSERT_MESSAGE(zNear < zFar, "zNear: out of range");

    // NOTE: Right-handed Cartesian Coordinates
    //
    // f = cotangent(fovY/2)
    // cotangent(fovY/2) = 1/tan(fovY/2)
    //
    // The generated matrix is:
    // (f/Aspect)  0   0               0
    // 0           f   0               0
    // 0           0   zf/(zn-zf)     -1
    // 0           0   zn*zf/(zn-zf)   0

    const auto yScale = 1.0f / std::tan(fovy.value * 0.5f);
    const auto xScale = yScale / aspect;
    const auto p = zFar / (zNear - zFar);

    return Matrix4x4{
        xScale,
        0.0f,
        0.0f,
        0.0f,

        0.0f,
        yScale,
        0.0f,
        0.0f,

        0.0f,
        0.0f,
        p,
        -1.0f,

        0.0f,
        0.0f,
        zNear * p,
        0.0f,
    };
}

Matrix4x4
Matrix4x4::createPerspectiveOffCenterLH(float left, float right, float bottom, float top, float zNear, float zFar)
{
    POMDOG_ASSERT_MESSAGE(zNear > 0, "zNear: out of range");
    POMDOG_ASSERT_MESSAGE(zFar > 0, "zFar: out of range");
    POMDOG_ASSERT_MESSAGE(zNear < zFar, "zNear: out of range");

    // NOTE: Left-handed Cartesian Coordinates
    //
    // 2*zn/(r-l)   0            0              0
    // 0            2*zn/(t-b)   0              0
    // (l+r)/(l-r)  (t+b)/(b-t)  zf/(zf-zn)     1
    // 0            0            zn*zf/(zn-zf)  0

    const auto x = 2.0f * zNear / (right - left);
    const auto y = 2.0f * zNear / (top - bottom);
    const auto a = (left + right) / (left - right); // RH: (left + right) / (right - left)
    const auto b = (top + bottom) / (bottom - top); // RH: (top + bottom) / (top - bottom)
    const auto c = zFar / (zFar - zNear);           // RH: zFar / (zNear - zFar)
    const auto d = zNear * zFar / (zNear - zFar);

    return Matrix4x4{
        x,
        0.0f,
        0.0f,
        0.0f,

        0.0f,
        y,
        0.0f,
        0.0f,

        a,
        b,
        c,
        1.0f, // RH: -1

        0.0f,
        0.0f,
        d,
        0.0f,
    };
}

Matrix4x4
Matrix4x4::createPerspectiveOffCenterRH(float left, float right, float bottom, float top, float zNear, float zFar)
{
    POMDOG_ASSERT_MESSAGE(zNear > 0, "zNear: out of range");
    POMDOG_ASSERT_MESSAGE(zFar > 0, "zFar: out of range");
    POMDOG_ASSERT_MESSAGE(zNear < zFar, "zNear: out of range");

    // NOTE: Right-handed Cartesian Coordinates
    //
    // 2*zn/(r-l)   0            0              0
    // 0            2*zn/(t-b)   0              0
    // (l+r)/(r-l)  (t+b)/(t-b)  zf/(zn-zf)    -1
    // 0            0            zn*zf/(zn-zf)  0

    const auto x = 2.0f * zNear / (right - left);
    const auto y = 2.0f * zNear / (top - bottom);
    const auto a = (left + right) / (right - left); // LH: (left + right) / (left - right)
    const auto b = (top + bottom) / (top - bottom); // LH: (top + bottom) / (bottom - top)
    const auto c = zFar / (zNear - zFar);           // LH: zFar / (zFar - zNear)
    const auto d = zNear * zFar / (zNear - zFar);

    return Matrix4x4{
        x,
        0.0f,
        0.0f,
        0.0f,

        0.0f,
        y,
        0.0f,
        0.0f,

        a,
        b,
        c,
        -1.0f, // LH: +1

        0.0f,
        0.0f,
        d,
        0.0f,
    };
}

Matrix4x4
Matrix4x4::createOrthographicOffCenterLH(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane)
{
    POMDOG_ASSERT((right - left) != 0);
    POMDOG_ASSERT((top - bottom) != 0);
    POMDOG_ASSERT(zNearPlane < zFarPlane);

    // NOTE: Left-handed Cartesian Coordinates
    //
    // 2/(r-l)      0            0           0
    // 0            2/(t-b)      0           0
    // 0            0            1/(zf-zn)   0
    // (l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  1

    const auto inverseRightLeft = 1.0f / (right - left);
    const auto inverseTopBottom = 1.0f / (top - bottom);
    const auto inverseFarNear = 1.0f / (zFarPlane - zNearPlane);

    return Matrix4x4{
        2.0f * inverseRightLeft,
        0.0f,
        0.0f,
        0.0f,

        0.0f,
        2.0f * inverseTopBottom,
        0.0f,
        0.0f,

        0.0f,
        0.0f,
        inverseFarNear,
        0.0f,

        (left + right) * (-inverseRightLeft),
        (top + bottom) * (-inverseTopBottom),
        zNearPlane * (-inverseFarNear),
        1.0f,
    };
}

Matrix4x4
Matrix4x4::createOrthographicOffCenterRH(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane)
{
    POMDOG_ASSERT((right - left) != 0);
    POMDOG_ASSERT((top - bottom) != 0);
    POMDOG_ASSERT(zNearPlane < zFarPlane);

    // NOTE: Right-handed Cartesian Coordinates
    //
    // 2/(r-l)      0            0           0
    // 0            2/(t-b)      0           0
    // 0            0            1/(zn-zf)   0
    // (l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  1

    const auto inverseRightLeft = 1.0f / (right - left);
    const auto inverseTopBottom = 1.0f / (top - bottom);
    const auto inverseFarNear = 1.0f / (zNearPlane - zFarPlane);

    return Matrix4x4{
        2.0f * inverseRightLeft,
        0.0f,
        0.0f,
        0.0f,

        0.0f,
        2.0f * inverseTopBottom,
        0.0f,
        0.0f,

        0.0f,
        0.0f,
        inverseFarNear,
        0.0f,

        (left + right) * (-inverseRightLeft),
        (top + bottom) * (-inverseTopBottom),
        zNearPlane * inverseFarNear,
        1.0f,
    };
}

Matrix4x4
Matrix4x4::createOrthographicLH(float width, float height, float zNearPlane, float zFarPlane)
{
    return createOrthographicOffCenterLH(
        -width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, zNearPlane, zFarPlane);
}

Matrix4x4
Matrix4x4::createOrthographicRH(float width, float height, float zNearPlane, float zFarPlane)
{
    return createOrthographicOffCenterRH(
        -width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, zNearPlane, zFarPlane);
}

Matrix4x4
Matrix4x4::createFromAxisAngle(const Vector3& axis, const Radian<float>& angle)
{
    const auto sinAngle = std::sin(angle.value);
    const auto cosAngle = std::cos(angle.value);
    const auto factor = 1.0f - cosAngle;
    const auto xx = axis.x * axis.x;
    const auto yy = axis.y * axis.y;
    const auto zz = axis.z * axis.z;
    const auto xy = axis.x * axis.y;
    const auto xz = axis.x * axis.z;
    const auto yz = axis.y * axis.z;

    return Matrix4x4{
        factor * xx + cosAngle,
        factor * xy + sinAngle * axis.z,
        factor * xz - sinAngle * axis.y,
        0.0f,
        factor * xy - sinAngle * axis.z,
        factor * yy + cosAngle,
        factor * yz + sinAngle * axis.x,
        0.0f,
        factor * xz + sinAngle * axis.y,
        factor * yz - sinAngle * axis.x,
        factor * zz + cosAngle,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        1.0f,
    };
}

const float* Matrix4x4::data() const noexcept
{
    return m[0];
}

float* Matrix4x4::data() noexcept
{
    return m[0];
}

[[nodiscard]] Matrix4x4
operator*(float factor, const Matrix4x4& matrix) noexcept
{
    return math::multiply(matrix, factor);
}

Matrix4x4 Matrix4x4::createIdentity() noexcept
{
    return Matrix4x4{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
}

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] float
determinant(const Matrix4x4& matrix) noexcept
{
    const auto& m = matrix.m;

    // TODO: Replace with simple process using cofactors to compute a determinant
    //       please see: Matrix3x3::Determinant implementation
    return m[0][0] * m[1][1] * m[2][2] * m[3][3] + m[0][0] * m[1][2] * m[2][3] * m[3][1] + m[0][0] * m[1][3] * m[2][1] * m[3][2] +
           m[0][1] * m[1][0] * m[2][3] * m[3][2] + m[0][1] * m[1][2] * m[2][0] * m[3][3] + m[0][1] * m[1][3] * m[2][2] * m[3][0] +
           m[0][2] * m[1][0] * m[2][1] * m[3][3] + m[0][2] * m[1][1] * m[2][3] * m[3][0] + m[0][2] * m[1][3] * m[2][0] * m[3][1] +
           m[0][3] * m[1][0] * m[2][2] * m[3][1] + m[0][3] * m[1][1] * m[2][0] * m[3][2] + m[0][3] * m[1][2] * m[2][1] * m[3][0] -
           m[0][0] * m[1][1] * m[2][3] * m[3][2] - m[0][0] * m[1][2] * m[2][1] * m[3][3] - m[0][0] * m[1][3] * m[2][2] * m[3][1] -
           m[0][1] * m[1][0] * m[2][2] * m[3][3] - m[0][1] * m[1][2] * m[2][3] * m[3][0] - m[0][1] * m[1][3] * m[2][0] * m[3][2] -
           m[0][2] * m[1][0] * m[2][3] * m[3][1] - m[0][2] * m[1][1] * m[2][0] * m[3][3] - m[0][2] * m[1][3] * m[2][1] * m[3][0] -
           m[0][3] * m[1][0] * m[2][1] * m[3][2] - m[0][3] * m[1][1] * m[2][2] * m[3][0] - m[0][3] * m[1][2] * m[2][0] * m[3][1];
}

[[nodiscard]] Matrix3x3
minor3x3(const Matrix4x4& matrix, std::size_t row, std::size_t column)
{
    static_assert(std::is_same_v<decltype(matrix.m), float[4][4]>);
    constexpr std::size_t rowSize = 4;
    constexpr std::size_t columnSize = 4;
    POMDOG_ASSERT_MESSAGE(row < rowSize, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < columnSize, "column: out of range");

    // NOTE:
    //      4x4 matrix                              3x3 minor
    //     c0  c1  c2  c3
    // r0 |11, 12, 13, 14|                       |11, 12, 13, x|
    // r1 |21, 22, 23, 24| Minor3x3(mat, r2, c3) |21, 22. 23, x|
    // r2 |31, 32, 33, 34| --------------------> |41, 42, 43, x|
    // r3 |41, 42, 43, 44|                       | x,  x,  x, x|

#if defined(_MSC_VER) && !defined(NDEBUG)
    // NOTE: Avoid MSVC warning C4701: potentially uninitialized local variable 'minorMatrix' used
    auto minorMatrix = Matrix3x3::createIdentity();
#else
    Matrix3x3 minorMatrix;
#endif
    for (std::size_t i = 0, s = 0; i < rowSize; ++i) {
        if (row == i) {
            continue;
        }

        for (std::size_t j = 0, t = 0; j < columnSize; ++j) {
            if (column == j) {
                continue;
            }

            minorMatrix.m[s][t] = matrix.m[i][j];
            ++t;
        }
        ++s;
    }
    return minorMatrix;
}

[[nodiscard]] Matrix4x4
multiply(const Matrix4x4& matrix1, const Matrix4x4& matrix2) noexcept
{
    const auto& a = matrix1.m;
    const auto& b = matrix2.m;
    return Matrix4x4{
        a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0] + a[0][3] * b[3][0],
        a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1] + a[0][3] * b[3][1],
        a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2] + a[0][3] * b[3][2],
        a[0][0] * b[0][3] + a[0][1] * b[1][3] + a[0][2] * b[2][3] + a[0][3] * b[3][3],

        a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0] + a[1][3] * b[3][0],
        a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1] + a[1][3] * b[3][1],
        a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2] + a[1][3] * b[3][2],
        a[1][0] * b[0][3] + a[1][1] * b[1][3] + a[1][2] * b[2][3] + a[1][3] * b[3][3],

        a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0] + a[2][3] * b[3][0],
        a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1] + a[2][3] * b[3][1],
        a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2] + a[2][3] * b[3][2],
        a[2][0] * b[0][3] + a[2][1] * b[1][3] + a[2][2] * b[2][3] + a[2][3] * b[3][3],

        a[3][0] * b[0][0] + a[3][1] * b[1][0] + a[3][2] * b[2][0] + a[3][3] * b[3][0],
        a[3][0] * b[0][1] + a[3][1] * b[1][1] + a[3][2] * b[2][1] + a[3][3] * b[3][1],
        a[3][0] * b[0][2] + a[3][1] * b[1][2] + a[3][2] * b[2][2] + a[3][3] * b[3][2],
        a[3][0] * b[0][3] + a[3][1] * b[1][3] + a[3][2] * b[2][3] + a[3][3] * b[3][3],
    };
}

[[nodiscard]] Matrix4x4
multiply(const Matrix4x4& matrix1, float factor) noexcept
{
    return Matrix4x4{
        matrix1.m[0][0] * factor,
        matrix1.m[0][1] * factor,
        matrix1.m[0][2] * factor,
        matrix1.m[0][3] * factor,
        matrix1.m[1][0] * factor,
        matrix1.m[1][1] * factor,
        matrix1.m[1][2] * factor,
        matrix1.m[1][3] * factor,
        matrix1.m[2][0] * factor,
        matrix1.m[2][1] * factor,
        matrix1.m[2][2] * factor,
        matrix1.m[2][3] * factor,
        matrix1.m[3][0] * factor,
        matrix1.m[3][1] * factor,
        matrix1.m[3][2] * factor,
        matrix1.m[3][3] * factor,
    };
}

[[nodiscard]] Matrix4x4
adjoint(const Matrix4x4& matrix)
{
    return Matrix4x4{
        +math::determinant(math::minor3x3(matrix, 0, 0)),
        -math::determinant(math::minor3x3(matrix, 1, 0)),
        +math::determinant(math::minor3x3(matrix, 2, 0)),
        -math::determinant(math::minor3x3(matrix, 3, 0)),
        -math::determinant(math::minor3x3(matrix, 0, 1)),
        +math::determinant(math::minor3x3(matrix, 1, 1)),
        -math::determinant(math::minor3x3(matrix, 2, 1)),
        +math::determinant(math::minor3x3(matrix, 3, 1)),
        +math::determinant(math::minor3x3(matrix, 0, 2)),
        -math::determinant(math::minor3x3(matrix, 1, 2)),
        +math::determinant(math::minor3x3(matrix, 2, 2)),
        -math::determinant(math::minor3x3(matrix, 3, 2)),
        -math::determinant(math::minor3x3(matrix, 0, 3)),
        +math::determinant(math::minor3x3(matrix, 1, 3)),
        -math::determinant(math::minor3x3(matrix, 2, 3)),
        +math::determinant(math::minor3x3(matrix, 3, 3)),
    };
}

[[nodiscard]] Matrix4x4
transpose(const Matrix4x4& matrix) noexcept
{
    return Matrix4x4{
        matrix.m[0][0], matrix.m[1][0], matrix.m[2][0], matrix.m[3][0],
        matrix.m[0][1], matrix.m[1][1], matrix.m[2][1], matrix.m[3][1],
        matrix.m[0][2], matrix.m[1][2], matrix.m[2][2], matrix.m[3][2],
        matrix.m[0][3], matrix.m[1][3], matrix.m[2][3], matrix.m[3][3]};
}

[[nodiscard]] Matrix4x4
invert(const Matrix4x4& matrix)
{
    const auto determinant = math::determinant(matrix);

    // NOTE: The 'matrix' must be non-singular matrix.
    POMDOG_ASSERT(std::fpclassify(determinant) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(determinant) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(determinant) != FP_INFINITE);

    const auto inverseDet = 1.0f / determinant;
    return adjoint(matrix) * inverseDet;
}

[[nodiscard]] Matrix4x4
lerp(const Matrix4x4& source1, const Matrix4x4& source2, float amount) noexcept
{
    return Matrix4x4{
        math::lerp(source1.m[0][0], source2.m[0][0], amount),
        math::lerp(source1.m[0][1], source2.m[0][1], amount),
        math::lerp(source1.m[0][2], source2.m[0][2], amount),
        math::lerp(source1.m[0][3], source2.m[0][3], amount),
        math::lerp(source1.m[1][0], source2.m[1][0], amount),
        math::lerp(source1.m[1][1], source2.m[1][1], amount),
        math::lerp(source1.m[1][2], source2.m[1][2], amount),
        math::lerp(source1.m[1][3], source2.m[1][3], amount),
        math::lerp(source1.m[2][0], source2.m[2][0], amount),
        math::lerp(source1.m[2][1], source2.m[2][1], amount),
        math::lerp(source1.m[2][2], source2.m[2][2], amount),
        math::lerp(source1.m[2][3], source2.m[2][3], amount),
        math::lerp(source1.m[3][0], source2.m[3][0], amount),
        math::lerp(source1.m[3][1], source2.m[3][1], amount),
        math::lerp(source1.m[3][2], source2.m[3][2], amount),
        math::lerp(source1.m[3][3], source2.m[3][3], amount),
    };
}

} // namespace pomdog::math
