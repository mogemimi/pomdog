// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/turn.h"
#include "pomdog/math/degree.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/math/radian.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::math {

template <typename T>
Turn<T>::Turn() noexcept = default;

template <typename T>
Turn<T>::Turn(T v) noexcept
    : value_(v)
{
    POMDOG_ASSERT(!std::isnan(value_));
    POMDOG_ASSERT(!std::isinf(value_));
}

template <typename T>
Turn<T>::operator T() const noexcept
{
    return value_;
}

template <typename T>
Turn<T>& Turn<T>::operator+=(Turn v) noexcept
{
    value_ += v.value_;
    return *this;
}

template <typename T>
Turn<T>& Turn<T>::operator-=(Turn v) noexcept
{
    value_ -= v.value_;
    return *this;
}

template <typename T>
Turn<T>& Turn<T>::operator*=(Turn v) noexcept
{
    value_ *= v.value_;
    return *this;
}

template <typename T>
Turn<T>& Turn<T>::operator/=(Turn v)
{
    POMDOG_ASSERT(v.value_ != T{0});
    value_ /= v.value_;
    return *this;
}

template <typename T>
Turn<T>& Turn<T>::operator*=(T scale) noexcept
{
    value_ *= scale;
    return *this;
}

template <typename T>
Turn<T>& Turn<T>::operator/=(T scale)
{
    POMDOG_ASSERT(scale != T{0});
    value_ /= scale;
    return *this;
}

template <typename T>
Turn<T> Turn<T>::operator+() const noexcept
{
    return {+value_};
}

template <typename T>
Turn<T> Turn<T>::operator-() const noexcept
{
    return {-value_};
}

template <typename T>
Turn<T> Turn<T>::operator+(Turn v) const noexcept
{
    return {value_ + v.value_};
}

template <typename T>
Turn<T> Turn<T>::operator-(Turn v) const noexcept
{
    return {value_ - v.value_};
}

template <typename T>
Turn<T> Turn<T>::operator*(Turn v) const noexcept
{
    return {value_ * v.value_};
}

template <typename T>
Turn<T> Turn<T>::operator/(Turn v) const
{
    POMDOG_ASSERT(v.value_ != T{0});
    return {value_ / v.value_};
}

template <typename T>
Turn<T> Turn<T>::operator*(T scale) const noexcept
{
    return {value_ * scale};
}

template <typename T>
Turn<T> Turn<T>::operator/(T scale) const
{
    POMDOG_ASSERT(scale != T{0});
    return {value_ / scale};
}

template <typename T>
bool Turn<T>::operator==(Turn v) const noexcept
{
    return value_ == v.value_;
}

template <typename T>
bool Turn<T>::operator!=(Turn v) const noexcept
{
    return value_ != v.value_;
}

template <typename T>
bool Turn<T>::operator==(T scalar) const noexcept
{
    return value_ == scalar;
}

template <typename T>
bool Turn<T>::operator!=(T scalar) const noexcept
{
    return value_ != scalar;
}

template <typename T>
bool Turn<T>::operator<(Turn v) const noexcept
{
    return value_ < v.value_;
}

template <typename T>
bool Turn<T>::operator>(Turn v) const noexcept
{
    return value_ > v.value_;
}

template <typename T>
bool Turn<T>::operator<=(Turn v) const noexcept
{
    return value_ <= v.value_;
}

template <typename T>
bool Turn<T>::operator>=(Turn v) const noexcept
{
    return value_ >= v.value_;
}

template <typename T>
bool Turn<T>::operator<(T scalar) const noexcept
{
    return value_ < scalar;
}

template <typename T>
bool Turn<T>::operator>(T scalar) const noexcept
{
    return value_ > scalar;
}

template <typename T>
bool Turn<T>::operator<=(T scalar) const noexcept
{
    return value_ <= scalar;
}

template <typename T>
bool Turn<T>::operator>=(T scalar) const noexcept
{
    return value_ >= scalar;
}

template <typename T>
auto Turn<T>::get() const noexcept -> T
{
    return value_;
}

template <typename T>
[[nodiscard]] auto operator*(T scaleFactor, Turn<T> v) noexcept -> Turn<T>
{
    return {scaleFactor * v.get()};
}

// NOTE: explicit instantiations
template class Turn<f32>;
template class Turn<f64>;

// NOTE: explicit instantiations
template auto operator* <f32>(f32, Turn<f32>) noexcept -> Turn<f32>;
template auto operator* <f64>(f64, Turn<f64>) noexcept -> Turn<f64>;

} // namespace pomdog::math

namespace pomdog::math::detail {
namespace {

template <typename T>
[[nodiscard]] auto toTurn(Radian<T> radian) noexcept -> Turn<T>
{
    static_assert(std::is_floating_point_v<T>);
    static_assert(math::TwoPi<T> > T{0});
    return Turn<T>{radian.get() / math::TwoPi<T>};
}

template <typename T>
[[nodiscard]] auto toTurn(Degree<T> degree) noexcept -> Turn<T>
{
    static_assert(std::is_floating_point_v<T>);
    return Turn<T>{degree.get() / T{360}};
}

template <typename T>
[[nodiscard]] auto normalizeSigned(Turn<T> turn) noexcept -> Turn<T>
{
    auto t = turn.get() - std::floor(turn.get());
    if (t > T{0.5}) {
        t -= T{1};
    }
    return Turn<T>{t};
}

template <typename T>
[[nodiscard]] auto normalizePositive(Turn<T> turn) noexcept -> Turn<T>
{
    return Turn<T>{turn.get() - std::floor(turn.get())};
}

} // namespace
} // namespace pomdog::math::detail

namespace pomdog::math {

[[nodiscard]] Turn<f32>
toTurn(Radian<f32> radian) noexcept
{
    return detail::toTurn(radian);
}

[[nodiscard]] Turn<f64>
toTurn(Radian<f64> radian) noexcept
{
    return detail::toTurn(radian);
}

[[nodiscard]] Turn<f32>
toTurn(Degree<f32> degree) noexcept
{
    return detail::toTurn(degree);
}

[[nodiscard]] Turn<f64>
toTurn(Degree<f64> degree) noexcept
{
    return detail::toTurn(degree);
}

[[nodiscard]] Turn<f32>
clamp(Turn<f32> x, Turn<f32> min, Turn<f32> max) noexcept
{
    return Turn<f32>{clamp(x.get(), min.get(), max.get())};
}

[[nodiscard]] Turn<f32>
saturate(Turn<f32> x) noexcept
{
    return Turn<f32>{saturate(x.get())};
}

[[nodiscard]] Turn<f32>
lerp(Turn<f32> source1, Turn<f32> source2, f32 amount) noexcept
{
    return Turn<f32>{lerp(source1.get(), source2.get(), amount)};
}

[[nodiscard]] Turn<f32>
smoothstep(Turn<f32> min, Turn<f32> max, f32 amount) noexcept
{
    return Turn<f32>{smoothstep(min.get(), max.get(), amount)};
}

[[nodiscard]] Turn<f32>
normalizeSigned(Turn<f32> turn) noexcept
{
    return detail::normalizeSigned(turn);
}

[[nodiscard]] Turn<f64>
normalizeSigned(Turn<f64> turn) noexcept
{
    return detail::normalizeSigned(turn);
}

[[nodiscard]] Turn<f32>
normalizePositive(Turn<f32> turn) noexcept
{
    return detail::normalizePositive(turn);
}

[[nodiscard]] Turn<f64>
normalizePositive(Turn<f64> turn) noexcept
{
    return detail::normalizePositive(turn);
}

} // namespace pomdog::math
