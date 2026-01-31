// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/degree.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/math/radian.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

template <typename T>
Degree<T>::Degree() noexcept = default;

template <typename T>
Degree<T>::Degree(T v) noexcept
    : value_(v)
{
    POMDOG_ASSERT(!std::isnan(value_));
    POMDOG_ASSERT(!std::isinf(value_));
}

template <typename T>
Degree<T>::operator T() const noexcept
{
    return value_;
}

template <typename T>
Degree<T>& Degree<T>::operator+=(Degree v) noexcept
{
    value_ += v.value_;
    return *this;
}

template <typename T>
Degree<T>& Degree<T>::operator-=(Degree v) noexcept
{
    value_ -= v.value_;
    return *this;
}

template <typename T>
Degree<T>& Degree<T>::operator*=(Degree v) noexcept
{
    value_ *= v.value_;
    return *this;
}

template <typename T>
Degree<T>& Degree<T>::operator/=(Degree v)
{
    POMDOG_ASSERT(v.value_ != T{0});
    value_ /= v.value_;
    return *this;
}

template <typename T>
Degree<T>& Degree<T>::operator*=(T scale) noexcept
{
    value_ *= scale;
    return *this;
}

template <typename T>
Degree<T>& Degree<T>::operator/=(T scale)
{
    POMDOG_ASSERT(scale != T{0});
    value_ /= scale;
    return *this;
}

template <typename T>
Degree<T> Degree<T>::operator+() const noexcept
{
    return {+value_};
}

template <typename T>
Degree<T> Degree<T>::operator-() const noexcept
{
    return {-value_};
}

template <typename T>
Degree<T> Degree<T>::operator+(Degree v) const noexcept
{
    return {value_ + v.value_};
}

template <typename T>
Degree<T> Degree<T>::operator-(Degree v) const noexcept
{
    return {value_ - v.value_};
}

template <typename T>
Degree<T> Degree<T>::operator*(Degree v) const noexcept
{
    return {value_ * v.value_};
}

template <typename T>
Degree<T> Degree<T>::operator/(Degree v) const
{
    POMDOG_ASSERT(v.value_ != T{0});
    return {value_ / v.value_};
}

template <typename T>
Degree<T> Degree<T>::operator*(T scale) const noexcept
{
    return {value_ * scale};
}

template <typename T>
Degree<T> Degree<T>::operator/(T scale) const
{
    POMDOG_ASSERT(scale != T{0});
    return {value_ / scale};
}

template <typename T>
bool Degree<T>::operator==(Degree v) const noexcept
{
    return value_ == v.value_;
}

template <typename T>
bool Degree<T>::operator!=(Degree v) const noexcept
{
    return value_ != v.value_;
}

template <typename T>
bool Degree<T>::operator==(T scalar) const noexcept
{
    return value_ == scalar;
}

template <typename T>
bool Degree<T>::operator!=(T scalar) const noexcept
{
    return value_ != scalar;
}

template <typename T>
bool Degree<T>::operator<(Degree v) const noexcept
{
    return value_ < v.value_;
}

template <typename T>
bool Degree<T>::operator>(Degree v) const noexcept
{
    return value_ > v.value_;
}

template <typename T>
bool Degree<T>::operator<=(Degree v) const noexcept
{
    return value_ <= v.value_;
}

template <typename T>
bool Degree<T>::operator>=(Degree v) const noexcept
{
    return value_ >= v.value_;
}

template <typename T>
bool Degree<T>::operator<(T scalar) const noexcept
{
    return value_ < scalar;
}

template <typename T>
bool Degree<T>::operator>(T scalar) const noexcept
{
    return value_ > scalar;
}

template <typename T>
bool Degree<T>::operator<=(T scalar) const noexcept
{
    return value_ <= scalar;
}

template <typename T>
bool Degree<T>::operator>=(T scalar) const noexcept
{
    return value_ >= scalar;
}

template <typename T>
auto Degree<T>::get() const noexcept -> T
{
    return value_;
}

template <typename T>
[[nodiscard]] auto operator*(T scaleFactor, Degree<T> v) noexcept -> Degree<T>
{
    return {scaleFactor * v.get()};
}

// NOTE: explicit instantiations
template class Degree<f32>;
template class Degree<f64>;

// NOTE: explicit instantiations
template auto operator* <f32>(f32, Degree<f32>) noexcept -> Degree<f32>;
template auto operator* <f64>(f64, Degree<f64>) noexcept -> Degree<f64>;

} // namespace pomdog

namespace pomdog::math::detail {
namespace {

template <typename T>
[[nodiscard]] auto toDegree(Radian<T> radian) noexcept -> Degree<T>
{
    static_assert(std::is_floating_point_v<T>);
    static_assert(math::Pi<T> > T{0});
    POMDOG_ASSERT(!std::isnan(radian.get()));
    POMDOG_ASSERT(!std::isinf(radian.get()));

    constexpr auto scaleFactor = T{180} / math::Pi<T>;
    return Degree<T>{radian.get() * scaleFactor};
}

template <typename T>
[[nodiscard]] auto toDegree(T radian) noexcept -> Degree<T>
{
    static_assert(std::is_floating_point_v<T>);
    static_assert(math::Pi<T> > T{0});
    POMDOG_ASSERT(!std::isnan(radian));
    POMDOG_ASSERT(!std::isinf(radian));

    constexpr auto scaleFactor = T{180} / math::Pi<T>;
    return Degree<T>{radian * scaleFactor};
}

} // namespace
} // namespace pomdog::math::detail

namespace pomdog::math {

[[nodiscard]] Degree<f32>
toDegree(Radian<f32> radian) noexcept
{
    return detail::toDegree(radian);
}

[[nodiscard]] Degree<f32>
toDegree(f32 radian) noexcept
{
    return detail::toDegree(radian);
}

[[nodiscard]] Degree<f64>
toDegree(Radian<f64> radian) noexcept
{
    return detail::toDegree(radian);
}

[[nodiscard]] Degree<f64>
toDegree(f64 radian) noexcept
{
    return detail::toDegree(radian);
}

[[nodiscard]] Degree<f32>
clamp(Degree<f32> x, Degree<f32> min, Degree<f32> max) noexcept
{
    return Degree<f32>{clamp(x.get(), min.get(), max.get())};
}

[[nodiscard]] Degree<f32>
saturate(Degree<f32> x) noexcept
{
    return Degree<f32>{saturate(x.get())};
}

[[nodiscard]] Degree<f32>
lerp(Degree<f32> source1, Degree<f32> source2, f32 amount) noexcept
{
    return Degree<f32>{lerp(source1.get(), source2.get(), amount)};
}

[[nodiscard]] Degree<f32>
smoothstep(Degree<f32> min, Degree<f32> max, f32 amount) noexcept
{
    return Degree<f32>{smoothstep(min.get(), max.get(), amount)};
}

} // namespace pomdog::math
