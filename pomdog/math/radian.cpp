// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/radian.h"
#include "pomdog/math/degree.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

template <typename T>
Radian<T>::Radian() noexcept = default;

template <typename T>
Radian<T>::Radian(T v) noexcept
    : value_(v)
{
    POMDOG_ASSERT(!std::isnan(value_));
    POMDOG_ASSERT(!std::isinf(value_));
}

template <typename T>
Radian<T>::operator T() const noexcept
{
    return value_;
}

template <typename T>
Radian<T>& Radian<T>::operator+=(Radian v) noexcept
{
    value_ += v.value_;
    return *this;
}

template <typename T>
Radian<T>& Radian<T>::operator-=(Radian v) noexcept
{
    value_ -= v.value_;
    return *this;
}

template <typename T>
Radian<T>& Radian<T>::operator*=(Radian v) noexcept
{
    value_ *= v.value_;
    return *this;
}

template <typename T>
Radian<T>& Radian<T>::operator/=(Radian v)
{
    POMDOG_ASSERT(v.value_ != T{0});
    value_ /= v.value_;
    return *this;
}

template <typename T>
Radian<T>& Radian<T>::operator*=(T scale) noexcept
{
    value_ *= scale;
    return *this;
}

template <typename T>
Radian<T>& Radian<T>::operator/=(T scale)
{
    POMDOG_ASSERT(scale != T{0});
    value_ /= scale;
    return *this;
}

template <typename T>
Radian<T> Radian<T>::operator+() const noexcept
{
    return {+value_};
}

template <typename T>
Radian<T> Radian<T>::operator-() const noexcept
{
    return {-value_};
}

template <typename T>
Radian<T> Radian<T>::operator+(Radian v) const noexcept
{
    return {value_ + v.value_};
}

template <typename T>
Radian<T> Radian<T>::operator-(Radian v) const noexcept
{
    return {value_ - v.value_};
}

template <typename T>
Radian<T> Radian<T>::operator*(Radian v) const noexcept
{
    return {value_ * v.value_};
}

template <typename T>
Radian<T> Radian<T>::operator/(Radian v) const
{
    POMDOG_ASSERT(v.value_ != T{0});
    return {value_ / v.value_};
}

template <typename T>
Radian<T> Radian<T>::operator*(T scale) const noexcept
{
    return {value_ * scale};
}

template <typename T>
Radian<T> Radian<T>::operator/(T scale) const
{
    POMDOG_ASSERT(scale != T{0});
    return {value_ / scale};
}

template <typename T>
bool Radian<T>::operator==(Radian v) const noexcept
{
    return value_ == v.value_;
}

template <typename T>
bool Radian<T>::operator!=(Radian v) const noexcept
{
    return value_ != v.value_;
}

template <typename T>
bool Radian<T>::operator==(T scalar) const noexcept
{
    return value_ == scalar;
}

template <typename T>
bool Radian<T>::operator!=(T scalar) const noexcept
{
    return value_ != scalar;
}

template <typename T>
bool Radian<T>::operator<(Radian v) const noexcept
{
    return value_ < v.value_;
}

template <typename T>
bool Radian<T>::operator>(Radian v) const noexcept
{
    return value_ > v.value_;
}

template <typename T>
bool Radian<T>::operator<=(Radian v) const noexcept
{
    return value_ <= v.value_;
}

template <typename T>
bool Radian<T>::operator>=(Radian v) const noexcept
{
    return value_ >= v.value_;
}

template <typename T>
bool Radian<T>::operator<(T scalar) const noexcept
{
    return value_ < scalar;
}

template <typename T>
bool Radian<T>::operator>(T scalar) const noexcept
{
    return value_ > scalar;
}

template <typename T>
bool Radian<T>::operator<=(T scalar) const noexcept
{
    return value_ <= scalar;
}

template <typename T>
bool Radian<T>::operator>=(T scalar) const noexcept
{
    return value_ >= scalar;
}

template <typename T>
auto Radian<T>::get() const noexcept -> T
{
    return value_;
}

template <typename T>
[[nodiscard]] auto operator*(T scaleFactor, Radian<T> v) noexcept -> Radian<T>
{
    return {scaleFactor * v.get()};
}

// NOTE: explicit instantiations
template class Radian<f32>;
template class Radian<f64>;

// NOTE: explicit instantiations
template auto operator* <f32>(f32, Radian<f32>) noexcept -> Radian<f32>;
template auto operator* <f64>(f64, Radian<f64>) noexcept -> Radian<f64>;

} // namespace pomdog

namespace pomdog::math::detail {
namespace {

template <typename T>
[[nodiscard]] auto toRadian(Degree<T> degree) noexcept -> Radian<T>
{
    static_assert(std::is_floating_point_v<T>);
    static_assert(math::Pi<T> > T{0});
    POMDOG_ASSERT(!std::isnan(degree.get()));
    POMDOG_ASSERT(!std::isinf(degree.get()));

    constexpr auto scaleFactor = math::Pi<T> / T{180};
    return Radian<T>{degree.get() * scaleFactor};
}

template <typename T>
[[nodiscard]] auto toRadian(T degree) noexcept -> Radian<T>
{
    static_assert(std::is_floating_point_v<T>);
    static_assert(math::Pi<T> > T{0});
    POMDOG_ASSERT(!std::isnan(degree));
    POMDOG_ASSERT(!std::isinf(degree));

    constexpr auto scaleFactor = math::Pi<T> / T{180};
    return Radian<T>{degree * scaleFactor};
}

template <typename T>
[[nodiscard]] auto normalizeSigned(Radian<T> radian) noexcept -> Radian<T>
{
    auto r = radian.get();
    while (r > math::Pi<T>) {
        r -= math::TwoPi<T>;
    }
    while (r < -math::Pi<T>) {
        r += math::TwoPi<T>;
    }
    return Radian<T>{r};
}

template <typename T>
[[nodiscard]] auto normalizePositive(Radian<T> radian) noexcept -> Radian<T>
{
    auto r = radian.get();
    while (r > math::TwoPi<T>) {
        r -= math::TwoPi<T>;
    }
    while (r < T{0}) {
        r += math::TwoPi<T>;
    }
    return Radian<T>{r};
}

} // namespace
} // namespace pomdog::math::detail

namespace pomdog::math {

[[nodiscard]] Radian<f32>
toRadian(Degree<f32> degree) noexcept
{
    return detail::toRadian(degree);
}

[[nodiscard]] Radian<f32>
toRadian(f32 degree) noexcept
{
    return detail::toRadian(degree);
}

[[nodiscard]] Radian<f64>
toRadian(Degree<f64> degree) noexcept
{
    return detail::toRadian(degree);
}

[[nodiscard]] Radian<f64>
toRadian(f64 degree) noexcept
{
    return detail::toRadian(degree);
}

[[nodiscard]] Radian<f32>
clamp(Radian<f32> x, Radian<f32> min, Radian<f32> max) noexcept
{
    return Radian<f32>{clamp(x.get(), min.get(), max.get())};
}

[[nodiscard]] Radian<f32>
saturate(Radian<f32> x) noexcept
{
    return Radian<f32>{saturate(x.get())};
}

[[nodiscard]] Radian<f32>
lerp(Radian<f32> source1, Radian<f32> source2, f32 amount) noexcept
{
    return Radian<f32>{lerp(source1.get(), source2.get(), amount)};
}

[[nodiscard]] Radian<f32>
smoothstep(Radian<f32> min, Radian<f32> max, f32 amount) noexcept
{
    return Radian<f32>{smoothstep(min.get(), max.get(), amount)};
}

[[nodiscard]] Radian<f32>
normalizeSigned(Radian<f32> radian) noexcept
{
    return detail::normalizeSigned(radian);
}

[[nodiscard]] Radian<f64>
normalizeSigned(Radian<f64> radian) noexcept
{
    return detail::normalizeSigned(radian);
}

[[nodiscard]] Radian<f32>
normalizePositive(Radian<f32> radian) noexcept
{
    return detail::normalizePositive(radian);
}

[[nodiscard]] Radian<f64>
normalizePositive(Radian<f64> radian) noexcept
{
    return detail::normalizePositive(radian);
}

} // namespace pomdog::math
