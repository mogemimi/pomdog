// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/detail/coordinate2d.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

template <typename T>
Coordinate2D<T>::Coordinate2D(T x, T y) noexcept
    : X(x)
    , Y(y)
{
}

template <typename T>
Coordinate2D<T>& Coordinate2D<T>::operator+=(const Coordinate2D& other) noexcept
{
    X += other.X;
    Y += other.Y;
    return *this;
}

template <typename T>
Coordinate2D<T>& Coordinate2D<T>::operator-=(const Coordinate2D& other) noexcept
{
    X -= other.X;
    Y -= other.Y;
    return *this;
}

template <typename T>
Coordinate2D<T>& Coordinate2D<T>::operator*=(T scaleFactor) noexcept
{
    X *= scaleFactor;
    Y *= scaleFactor;
    return *this;
}

template <typename T>
Coordinate2D<T>& Coordinate2D<T>::operator/=(T scaleFactor)
{
    POMDOG_ASSERT(scaleFactor != 0);
    X /= scaleFactor;
    Y /= scaleFactor;
    return *this;
}

template <typename T>
Coordinate2D<T> Coordinate2D<T>::operator+() const noexcept
{
    return *this;
}

template <typename T>
Coordinate2D<T> Coordinate2D<T>::operator-() const noexcept
{
    return {-X, -Y};
}

template <typename T>
Coordinate2D<T> Coordinate2D<T>::operator+(const Coordinate2D& other) const noexcept
{
    return {X + other.X, Y + other.Y};
}

template <typename T>
Coordinate2D<T> Coordinate2D<T>::operator-(const Coordinate2D& other) const noexcept
{
    return {X - other.X, Y - other.Y};
}

template <typename T>
Coordinate2D<T> Coordinate2D<T>::operator*(const Coordinate2D& other) const noexcept
{
    return {X * other.X, Y * other.Y};
}

template <typename T>
Coordinate2D<T> Coordinate2D<T>::operator/(const Coordinate2D& other) const
{
    return {X / other.X, Y / other.Y};
}

template <typename T>
Coordinate2D<T> Coordinate2D<T>::operator*(T scaleFactor) const noexcept
{
    return {X * scaleFactor, Y * scaleFactor};
}

template <typename T>
Coordinate2D<T> Coordinate2D<T>::operator/(T scaleFactor) const
{
    POMDOG_ASSERT(scaleFactor != 0);
    return {X / scaleFactor, Y / scaleFactor};
}

template <typename T>
bool Coordinate2D<T>::operator==(const Coordinate2D& other) const noexcept
{
    return (X == other.X && Y == other.Y);
}

template <typename T>
bool Coordinate2D<T>::operator!=(const Coordinate2D& other) const noexcept
{
    return (X != other.X || Y != other.Y);
}

template <typename T>
Coordinate2D<T> operator*(T scaleFactor, const Coordinate2D<T>& coordinate) noexcept
{
    return {scaleFactor * coordinate.X, scaleFactor * coordinate.Y};
}

template <typename T>
Coordinate2D<T> operator/(T scaleFactor, const Coordinate2D<T>& coordinate)
{
    POMDOG_ASSERT(coordinate.X != 0);
    POMDOG_ASSERT(coordinate.Y != 0);
    return {scaleFactor / coordinate.X, scaleFactor / coordinate.Y};
}

// explicit instantiations
template class Coordinate2D<std::int32_t>;

template Coordinate2D<std::int32_t>
operator*<std::int32_t>(std::int32_t, const Coordinate2D<std::int32_t>&) noexcept;

template Coordinate2D<std::int32_t>
operator/<std::int32_t>(std::int32_t, const Coordinate2D<std::int32_t>&);

} // namespace pomdog::detail
