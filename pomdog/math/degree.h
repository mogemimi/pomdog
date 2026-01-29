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
class Radian;
} // namespace pomdog

namespace pomdog {

template <typename T>
class POMDOG_EXPORT Degree final {
private:
    static_assert(std::is_floating_point_v<T>, "T is floating point type.");
    T value_;

public:
    Degree() noexcept;

    Degree(T v) noexcept;

    [[nodiscard]] explicit operator T() const noexcept;

    // Assignment operators:
    Degree& operator+=(Degree v) noexcept;

    Degree& operator-=(Degree v) noexcept;

    Degree& operator*=(Degree v) noexcept;

    Degree& operator/=(Degree v);

    Degree& operator*=(T scale) noexcept;

    Degree& operator/=(T scale);

    // Unary operators:
    [[nodiscard]] Degree operator+() const noexcept;

    [[nodiscard]] Degree operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Degree operator+(Degree v) const noexcept;

    [[nodiscard]] Degree operator-(Degree v) const noexcept;

    [[nodiscard]] Degree operator*(Degree v) const noexcept;

    [[nodiscard]] Degree operator/(Degree v) const;

    [[nodiscard]] Degree operator*(T scale) const noexcept;

    [[nodiscard]] Degree operator/(T scale) const;

    [[nodiscard]] bool operator==(Degree v) const noexcept;

    [[nodiscard]] bool operator!=(Degree v) const noexcept;

    [[nodiscard]] bool operator==(T scalar) const noexcept;

    [[nodiscard]] bool operator!=(T scalar) const noexcept;

    [[nodiscard]] bool operator<(Degree v) const noexcept;

    [[nodiscard]] bool operator>(Degree v) const noexcept;

    [[nodiscard]] bool operator<=(Degree v) const noexcept;

    [[nodiscard]] bool operator>=(Degree v) const noexcept;

    [[nodiscard]] bool operator<(T scalar) const noexcept;

    [[nodiscard]] bool operator>(T scalar) const noexcept;

    [[nodiscard]] bool operator<=(T scalar) const noexcept;

    [[nodiscard]] bool operator>=(T scalar) const noexcept;

    [[nodiscard]] T get() const noexcept;
};

template <typename T>
[[nodiscard]] auto operator*(T scaleFactor, Degree<T> v) noexcept -> Degree<T>;

extern template class Degree<f32>;
extern template class Degree<f64>;

using Degree32 = Degree<f32>;
using Degree64 = Degree<f64>;

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] Degree<f32> POMDOG_EXPORT
toDegrees(Radian<f32> radians) noexcept;

[[nodiscard]] Degree<f32> POMDOG_EXPORT
toDegrees(f32 radians) noexcept;

[[nodiscard]] Degree<f32> POMDOG_EXPORT
clamp(Degree<f32> x, Degree<f32> min, Degree<f32> max) noexcept;

[[nodiscard]] Degree<f32> POMDOG_EXPORT
saturate(Degree<f32> x) noexcept;

[[nodiscard]] Degree<f32> POMDOG_EXPORT
lerp(Degree<f32> source1, Degree<f32> source2, f32 amount) noexcept;

[[nodiscard]] Degree<f32> POMDOG_EXPORT
smoothstep(Degree<f32> min, Degree<f32> max, f32 amount) noexcept;

} // namespace pomdog::math
