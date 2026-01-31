// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
template <typename T>
class Degree;
} // namespace pomdog

namespace pomdog {

/// Represents an angle in radians.
/// It functions as a phantom type to prevent confusion with the Degree type.
template <typename T>
class POMDOG_EXPORT Radian final {
private:
    static_assert(std::is_floating_point_v<T>, "T is floating point type.");
    T value_ = {};

public:
    Radian() noexcept;

    Radian(T v) noexcept;

    [[nodiscard]] explicit operator T() const noexcept;

    // Assignment operators:
    Radian& operator+=(Radian v) noexcept;

    Radian& operator-=(Radian v) noexcept;

    Radian& operator*=(Radian v) noexcept;

    Radian& operator/=(Radian v);

    Radian& operator*=(T scale) noexcept;

    Radian& operator/=(T scale);

    // Unary operators:
    [[nodiscard]] Radian operator+() const noexcept;

    [[nodiscard]] Radian operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Radian operator+(Radian v) const noexcept;

    [[nodiscard]] Radian operator-(Radian v) const noexcept;

    [[nodiscard]] Radian operator*(Radian v) const noexcept;

    [[nodiscard]] Radian operator/(Radian v) const;

    [[nodiscard]] Radian operator*(T scale) const noexcept;

    [[nodiscard]] Radian operator/(T scale) const;

    [[nodiscard]] bool operator==(Radian v) const noexcept;

    [[nodiscard]] bool operator!=(Radian v) const noexcept;

    [[nodiscard]] bool operator==(T scalar) const noexcept;

    [[nodiscard]] bool operator!=(T scalar) const noexcept;

    [[nodiscard]] bool operator<(Radian v) const noexcept;

    [[nodiscard]] bool operator>(Radian v) const noexcept;

    [[nodiscard]] bool operator<=(Radian v) const noexcept;

    [[nodiscard]] bool operator>=(Radian v) const noexcept;

    [[nodiscard]] bool operator<(T scalar) const noexcept;

    [[nodiscard]] bool operator>(T scalar) const noexcept;

    [[nodiscard]] bool operator<=(T scalar) const noexcept;

    [[nodiscard]] bool operator>=(T scalar) const noexcept;

    /// Gets the underlying value.
    [[nodiscard]] T get() const noexcept;
};

template <typename T>
[[nodiscard]] auto operator*(T scaleFactor, Radian<T> v) noexcept -> Radian<T>;

extern template class Radian<f32>;
extern template class Radian<f64>;

using Radian32 = Radian<f32>;
using Radian64 = Radian<f64>;

} // namespace pomdog

namespace pomdog::math {

/// Converts degrees to radians.
[[nodiscard]] Radian<f32> POMDOG_EXPORT
toRadian(Degree<f32> degree) noexcept;

/// Converts degrees to radians.
[[nodiscard]] Radian<f32> POMDOG_EXPORT
toRadian(f32 degree) noexcept;

/// Converts degrees to radians.
[[nodiscard]] Radian<f64> POMDOG_EXPORT
toRadian(Degree<f64> degree) noexcept;

/// Converts degrees to radians.
[[nodiscard]] Radian<f64> POMDOG_EXPORT
toRadian(f64 degree) noexcept;

/// Clamps a value between a minimum and maximum value.
[[nodiscard]] Radian<f32> POMDOG_EXPORT
clamp(Radian<f32> x, Radian<f32> min, Radian<f32> max) noexcept;

/// Saturates a value to the range [0, 1].
[[nodiscard]] Radian<f32> POMDOG_EXPORT
saturate(Radian<f32> x) noexcept;

/// Performs a linear interpolation between two values.
[[nodiscard]] Radian<f32> POMDOG_EXPORT
lerp(Radian<f32> source1, Radian<f32> source2, f32 amount) noexcept;

/// Performs a smooth Hermite interpolation between two values.
[[nodiscard]] Radian<f32> POMDOG_EXPORT
smoothstep(Radian<f32> min, Radian<f32> max, f32 amount) noexcept;

/// Normalizes the angle to the range [-π, π].
[[nodiscard]] Radian<f32> POMDOG_EXPORT
normalizeSigned(Radian<f32> radian) noexcept;

/// Normalizes the angle to the range [-π, π].
[[nodiscard]] Radian<f64> POMDOG_EXPORT
normalizeSigned(Radian<f64> radian) noexcept;

/// Normalizes the angle to the range [0, 2π].
[[nodiscard]] Radian<f32> POMDOG_EXPORT
normalizePositive(Radian<f32> radian) noexcept;

/// Normalizes the angle to the range [0, 2π].
[[nodiscard]] Radian<f64> POMDOG_EXPORT
normalizePositive(Radian<f64> radian) noexcept;

} // namespace pomdog::math
