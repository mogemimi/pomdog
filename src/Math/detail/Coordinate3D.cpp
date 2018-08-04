// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/detail/Coordinate3D.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cstdint>

namespace Pomdog {
namespace Detail {

template <typename T>
Coordinate3D<T>::Coordinate3D(T x, T y, T z) noexcept
    : X(x)
    , Y(y)
    , Z(z)
{
}

template <typename T>
Coordinate3D<T> & Coordinate3D<T>::operator+=(const Coordinate3D& other) noexcept
{
    X += other.X;
    Y += other.Y;
    Z += other.Z;
    return *this;
}

template <typename T>
Coordinate3D<T> & Coordinate3D<T>::operator-=(const Coordinate3D& other) noexcept
{
    X -= other.X;
    Y -= other.Y;
    Z -= other.Z;
    return *this;
}

template <typename T>
Coordinate3D<T> & Coordinate3D<T>::operator*=(T scaleFactor) noexcept
{
    X *= scaleFactor;
    Y *= scaleFactor;
    Z *= scaleFactor;
    return *this;
}

template <typename T>
Coordinate3D<T> & Coordinate3D<T>::operator/=(T scaleFactor)
{
    POMDOG_ASSERT(scaleFactor != 0);
    X /= scaleFactor;
    Y /= scaleFactor;
    Z /= scaleFactor;
    return *this;
}

template <typename T>
Coordinate3D<T> Coordinate3D<T>::operator+() const noexcept
{
    return *this;
}

template <typename T>
Coordinate3D<T> Coordinate3D<T>::operator-() const noexcept
{
    return {-X, -Y, -Z};
}

template <typename T>
Coordinate3D<T> Coordinate3D<T>::operator+(const Coordinate3D& other) const noexcept
{
    return {X + other.X, Y + other.Y, Z + other.Z};
}

template <typename T>
Coordinate3D<T> Coordinate3D<T>::operator-(const Coordinate3D& other) const noexcept
{
    return {X - other.X, Y - other.Y, Z - other.Z};
}

template <typename T>
Coordinate3D<T> Coordinate3D<T>::operator*(const Coordinate3D& other) const noexcept
{
    return {X * other.X, Y * other.Y, Z * other.Z};
}

template <typename T>
Coordinate3D<T> Coordinate3D<T>::operator/(const Coordinate3D& other) const
{
    POMDOG_ASSERT(other.X != 0);
    POMDOG_ASSERT(other.Y != 0);
    POMDOG_ASSERT(other.Z != 0);
    return {X / other.X, Y / other.Y, Z / other.Z};
}

template <typename T>
Coordinate3D<T> Coordinate3D<T>::operator*(T scaleFactor) const noexcept
{
    return {X * scaleFactor, Y * scaleFactor, Z * scaleFactor};
}

template <typename T>
Coordinate3D<T> Coordinate3D<T>::operator/(T scaleFactor) const
{
    POMDOG_ASSERT(scaleFactor != 0);
    return {X / scaleFactor, Y / scaleFactor, Z / scaleFactor};
}

template <typename T>
bool Coordinate3D<T>::operator==(const Coordinate3D& other) const noexcept
{
    return (X == other.X && Y == other.Y && Z == other.Z);
}

template <typename T>
bool Coordinate3D<T>::operator!=(const Coordinate3D& other) const noexcept
{
    return (X != other.X || Y != other.Y || Z != other.Z);
}

template <typename T>
Coordinate3D<T> operator*(T scaleFactor, const Coordinate3D<T>& coordinate) noexcept
{
    return Coordinate3D<T>{
        scaleFactor * coordinate.X,
        scaleFactor * coordinate.Y,
        scaleFactor * coordinate.Z};
}

template <typename T>
Coordinate3D<T> operator/(T scaleFactor, const Coordinate3D<T>& coordinate)
{
    POMDOG_ASSERT(coordinate.X != 0);
    POMDOG_ASSERT(coordinate.Y != 0);
    POMDOG_ASSERT(coordinate.Z != 0);

    return Coordinate3D<T>{
        scaleFactor / coordinate.X,
        scaleFactor / coordinate.Y,
        scaleFactor / coordinate.Z};
}

// explicit instantiations
template class Coordinate3D<std::int32_t>;

template Coordinate3D<std::int32_t>
operator*<std::int32_t>(std::int32_t, const Coordinate3D<std::int32_t>&) noexcept;

template Coordinate3D<std::int32_t>
operator/<std::int32_t>(std::int32_t, const Coordinate3D<std::int32_t>&);

} // namespace Detail
} // namespace Pomdog
