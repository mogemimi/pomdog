// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/detail/Coordinate2D.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cstdint>

namespace Pomdog {
namespace Detail {

template <typename T>
Coordinate2D<T> const Coordinate2D<T>::Zero(0, 0);
//-----------------------------------------------------------------------
template <typename T>
Coordinate2D<T>::Coordinate2D(T x, T y) noexcept
    : X(x), Y(y)
{}
//-----------------------------------------------------------------------
template <typename T>
Coordinate2D<T> & Coordinate2D<T>::operator+=(Coordinate2D const& other) noexcept
{
    X += other.X;
    Y += other.Y;
    return *this;
}
//-----------------------------------------------------------------------
template <typename T>
Coordinate2D<T> & Coordinate2D<T>::operator-=(Coordinate2D const& other) noexcept
{
    X -= other.X;
    Y -= other.Y;
    return *this;
}
//-----------------------------------------------------------------------
template <typename T>
Coordinate2D<T> & Coordinate2D<T>::operator*=(T scaleFactor) noexcept
{
    X *= scaleFactor;
    Y *= scaleFactor;
    return *this;
}
//-----------------------------------------------------------------------
template <typename T>
Coordinate2D<T> & Coordinate2D<T>::operator/=(T scaleFactor)
{
    POMDOG_ASSERT(scaleFactor != 0);
    X /= scaleFactor;
    Y /= scaleFactor;
    return *this;
}
//-----------------------------------------------------------------------
template <typename T>
Coordinate2D<T> Coordinate2D<T>::operator+() const noexcept
{
    return *this;
}
//-----------------------------------------------------------------------
template <typename T>
Coordinate2D<T> Coordinate2D<T>::operator-() const noexcept
{
    return {-X, -Y};
}
//-----------------------------------------------------------------------
template <typename T>
Coordinate2D<T> Coordinate2D<T>::operator+(Coordinate2D const& other) const noexcept
{
    return {X + other.X, Y + other.Y};
}
//-----------------------------------------------------------------------
template <typename T>
Coordinate2D<T> Coordinate2D<T>::operator-(Coordinate2D const& other) const noexcept
{
    return {X - other.X, Y - other.Y};
}
//-----------------------------------------------------------------------
template <typename T>
Coordinate2D<T> Coordinate2D<T>::operator*(Coordinate2D const& other) const noexcept
{
    return {X * other.X, Y * other.Y};
}
//-----------------------------------------------------------------------
template <typename T>
Coordinate2D<T> Coordinate2D<T>::operator/(Coordinate2D const& other) const
{
    return {X / other.X, Y / other.Y};
}
//-----------------------------------------------------------------------
template <typename T>
Coordinate2D<T> Coordinate2D<T>::operator*(T scaleFactor) const noexcept
{
    return {X * scaleFactor, Y * scaleFactor};
}
//-----------------------------------------------------------------------
template <typename T>
Coordinate2D<T> Coordinate2D<T>::operator/(T scaleFactor) const
{
    POMDOG_ASSERT(scaleFactor != 0);
    return {X / scaleFactor, Y / scaleFactor};
}
//-----------------------------------------------------------------------
template <typename T>
bool Coordinate2D<T>::operator==(Coordinate2D const& other) const noexcept
{
    return (X == other.X && Y == other.Y);
}
//-----------------------------------------------------------------------
template <typename T>
bool Coordinate2D<T>::operator!=(Coordinate2D const& other) const noexcept
{
    return (X != other.X || Y != other.Y);
}
//-----------------------------------------------------------------------
template <typename T>
Coordinate2D<T> operator*(T scaleFactor, Coordinate2D<T> const& coordinate) noexcept
{
    return {scaleFactor * coordinate.X, scaleFactor * coordinate.Y};
}
//-----------------------------------------------------------------------
template <typename T>
Coordinate2D<T> operator/(T scaleFactor, Coordinate2D<T> const& coordinate)
{
    POMDOG_ASSERT(coordinate.X != 0);
    POMDOG_ASSERT(coordinate.Y != 0);
    return {scaleFactor / coordinate.X, scaleFactor / coordinate.Y};
}
//-----------------------------------------------------------------------
// explicit instantiations
template class Coordinate2D<std::int32_t>;

template Coordinate2D<std::int32_t>
operator*<std::int32_t>(std::int32_t, Coordinate2D<std::int32_t> const&) noexcept;

template Coordinate2D<std::int32_t>
operator/<std::int32_t>(std::int32_t, Coordinate2D<std::int32_t> const&);

} // namespace Detail
} // namespace Pomdog
