// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <type_traits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
template <typename T>
class Degree;
} // namespace pomdog

namespace pomdog {

template <typename T>
class POMDOG_EXPORT Radian final {
private:
    static_assert(std::is_floating_point_v<T>, "T is floating point type.");
    T value_;

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

[[nodiscard]] Radian<f32> POMDOG_EXPORT
toRadians(Degree<f32> degrees) noexcept;

[[nodiscard]] Radian<f32> POMDOG_EXPORT
toRadians(f32 degrees) noexcept;

[[nodiscard]] Radian<f32> POMDOG_EXPORT
clamp(Radian<f32> x, Radian<f32> min, Radian<f32> max) noexcept;

[[nodiscard]] Radian<f32> POMDOG_EXPORT
saturate(Radian<f32> x) noexcept;

[[nodiscard]] Radian<f32> POMDOG_EXPORT
lerp(Radian<f32> source1, Radian<f32> source2, f32 amount) noexcept;

[[nodiscard]] Radian<f32> POMDOG_EXPORT
smoothstep(Radian<f32> min, Radian<f32> max, f32 amount) noexcept;

} // namespace pomdog::math
