// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::math {
template <typename T>
class Degree;
template <typename T>
class Radian;
} // namespace pomdog::math

namespace pomdog::math {

/// Represents an angle in turns, where 1 turn equals 360 degrees or 2 pi radians.
/// It functions as a phantom type to prevent confusion with the Radian and Degree types.
template <typename T>
class POMDOG_EXPORT Turn final {
private:
    static_assert(std::is_floating_point_v<T>, "T is floating point type.");
    T value_ = {};

public:
    Turn() noexcept;

    Turn(T v) noexcept;

    [[nodiscard]] explicit operator T() const noexcept;

    // Assignment operators:
    Turn& operator+=(Turn v) noexcept;

    Turn& operator-=(Turn v) noexcept;

    Turn& operator*=(Turn v) noexcept;

    Turn& operator/=(Turn v);

    Turn& operator*=(T scale) noexcept;

    Turn& operator/=(T scale);

    // Unary operators:
    [[nodiscard]] Turn operator+() const noexcept;

    [[nodiscard]] Turn operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Turn operator+(Turn v) const noexcept;

    [[nodiscard]] Turn operator-(Turn v) const noexcept;

    [[nodiscard]] Turn operator*(Turn v) const noexcept;

    [[nodiscard]] Turn operator/(Turn v) const;

    [[nodiscard]] Turn operator*(T scale) const noexcept;

    [[nodiscard]] Turn operator/(T scale) const;

    [[nodiscard]] bool operator==(Turn v) const noexcept;

    [[nodiscard]] bool operator!=(Turn v) const noexcept;

    [[nodiscard]] bool operator==(T scalar) const noexcept;

    [[nodiscard]] bool operator!=(T scalar) const noexcept;

    [[nodiscard]] bool operator<(Turn v) const noexcept;

    [[nodiscard]] bool operator>(Turn v) const noexcept;

    [[nodiscard]] bool operator<=(Turn v) const noexcept;

    [[nodiscard]] bool operator>=(Turn v) const noexcept;

    [[nodiscard]] bool operator<(T scalar) const noexcept;

    [[nodiscard]] bool operator>(T scalar) const noexcept;

    [[nodiscard]] bool operator<=(T scalar) const noexcept;

    [[nodiscard]] bool operator>=(T scalar) const noexcept;

    /// Gets the underlying value.
    [[nodiscard]] T get() const noexcept;
};

template <typename T>
[[nodiscard]] auto operator*(T scaleFactor, Turn<T> v) noexcept -> Turn<T>;

extern template class Turn<f32>;
extern template class Turn<f64>;

extern template auto operator* <f32>(f32, Turn<f32>) noexcept -> Turn<f32>;
extern template auto operator* <f64>(f64, Turn<f64>) noexcept -> Turn<f64>;

} // namespace pomdog::math

namespace pomdog {

/// Represents an angle in turns as a 32-bit floating-point value.
using Turn32 = math::Turn<f32>;

/// Represents an angle in turns as a 64-bit floating-point value.
using Turn64 = math::Turn<f64>;

} // namespace pomdog

namespace pomdog::math {

/// Converts radians to turns.
[[nodiscard]] POMDOG_EXPORT Turn<f32>
toTurn(Radian<f32> radian) noexcept;

/// Converts radians to turns.
[[nodiscard]] POMDOG_EXPORT Turn<f64>
toTurn(Radian<f64> radian) noexcept;

/// Converts degrees to turns.
[[nodiscard]] POMDOG_EXPORT Turn<f32>
toTurn(Degree<f32> degree) noexcept;

/// Converts degrees to turns.
[[nodiscard]] POMDOG_EXPORT Turn<f64>
toTurn(Degree<f64> degree) noexcept;

/// Clamps a value between a minimum and maximum value.
[[nodiscard]] POMDOG_EXPORT Turn<f32>
clamp(Turn<f32> x, Turn<f32> min, Turn<f32> max) noexcept;

/// Saturates a value to the range [0, 1].
[[nodiscard]] POMDOG_EXPORT Turn<f32>
saturate(Turn<f32> x) noexcept;

/// Performs a linear interpolation between two values.
[[nodiscard]] POMDOG_EXPORT Turn<f32>
lerp(Turn<f32> source1, Turn<f32> source2, f32 amount) noexcept;

/// Performs a smooth Hermite interpolation between two values.
[[nodiscard]] POMDOG_EXPORT Turn<f32>
smoothstep(Turn<f32> min, Turn<f32> max, f32 amount) noexcept;

/// Normalizes the angle to the range [-0.5, 0.5].
[[nodiscard]] POMDOG_EXPORT Turn<f32>
normalizeSigned(Turn<f32> turn) noexcept;

/// Normalizes the angle to the range [-0.5, 0.5].
[[nodiscard]] POMDOG_EXPORT Turn<f64>
normalizeSigned(Turn<f64> turn) noexcept;

/// Normalizes the angle to the range [0, 1].
[[nodiscard]] POMDOG_EXPORT Turn<f32>
normalizePositive(Turn<f32> turn) noexcept;

/// Normalizes the angle to the range [0, 1].
[[nodiscard]] POMDOG_EXPORT Turn<f64>
normalizePositive(Turn<f64> turn) noexcept;

} // namespace pomdog::math
