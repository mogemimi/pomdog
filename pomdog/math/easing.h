// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

// NOTE: Easing functions compute interpolation weights from normalized time.
//
// The input `x` represents normalized time. It is typically in the `[0, 1]`
// range, but easing functions do not clamp or saturate `x`. Values outside
// `[0, 1]` may be useful for extrapolation or custom animation effects.
//
// The returned value is not guaranteed to be in the `[0, 1]` range. Some easing
// functions, such as Back, Elastic, and Bounce variants, may overshoot even when
// `x` is in the `[0, 1]` range.

namespace pomdog::math {

/// Returns `x` unchanged (no easing).
///
/// This function is useful for selecting "no easing" through the same interface
/// as other easing functions, such as an easing function pointer.
[[nodiscard]] POMDOG_EXPORT f32
easeLinear(f32 x) noexcept;

/// Returns `x` unchanged (no easing).
///
/// This function is useful for selecting "no easing" through the same interface
/// as other easing functions, such as an easing function pointer.
[[nodiscard]] POMDOG_EXPORT f64
easeLinear(f64 x) noexcept;

// MARK: Quadratic (Quad)

/// Returns a quadratic easing value that accelerates from zero.
[[nodiscard]] POMDOG_EXPORT f32
easeInQuad(f32 x) noexcept;

/// Returns a quadratic easing value that accelerates from zero.
[[nodiscard]] POMDOG_EXPORT f64
easeInQuad(f64 x) noexcept;

/// Returns a quadratic easing value that decelerates to zero.
[[nodiscard]] POMDOG_EXPORT f32
easeOutQuad(f32 x) noexcept;

/// Returns a quadratic easing value that decelerates to zero.
[[nodiscard]] POMDOG_EXPORT f64
easeOutQuad(f64 x) noexcept;

/// Returns a quadratic easing value that accelerates then decelerates.
[[nodiscard]] POMDOG_EXPORT f32
easeInOutQuad(f32 x) noexcept;

/// Returns a quadratic easing value that accelerates then decelerates.
[[nodiscard]] POMDOG_EXPORT f64
easeInOutQuad(f64 x) noexcept;

// MARK: Cubic

/// Returns a cubic easing value that accelerates from zero.
[[nodiscard]] POMDOG_EXPORT f32
easeInCubic(f32 x) noexcept;

/// Returns a cubic easing value that accelerates from zero.
[[nodiscard]] POMDOG_EXPORT f64
easeInCubic(f64 x) noexcept;

/// Returns a cubic easing value that decelerates to zero.
[[nodiscard]] POMDOG_EXPORT f32
easeOutCubic(f32 x) noexcept;

/// Returns a cubic easing value that decelerates to zero.
[[nodiscard]] POMDOG_EXPORT f64
easeOutCubic(f64 x) noexcept;

/// Returns a cubic easing value that accelerates then decelerates.
[[nodiscard]] POMDOG_EXPORT f32
easeInOutCubic(f32 x) noexcept;

/// Returns a cubic easing value that accelerates then decelerates.
[[nodiscard]] POMDOG_EXPORT f64
easeInOutCubic(f64 x) noexcept;

// MARK: Quartic (Quart)

/// Returns a quartic easing value that accelerates from zero.
[[nodiscard]] POMDOG_EXPORT f32
easeInQuart(f32 x) noexcept;

/// Returns a quartic easing value that accelerates from zero.
[[nodiscard]] POMDOG_EXPORT f64
easeInQuart(f64 x) noexcept;

/// Returns a quartic easing value that decelerates to zero.
[[nodiscard]] POMDOG_EXPORT f32
easeOutQuart(f32 x) noexcept;

/// Returns a quartic easing value that decelerates to zero.
[[nodiscard]] POMDOG_EXPORT f64
easeOutQuart(f64 x) noexcept;

/// Returns a quartic easing value that accelerates then decelerates.
[[nodiscard]] POMDOG_EXPORT f32
easeInOutQuart(f32 x) noexcept;

/// Returns a quartic easing value that accelerates then decelerates.
[[nodiscard]] POMDOG_EXPORT f64
easeInOutQuart(f64 x) noexcept;

// MARK: Quintic (Quint)

/// Returns a quintic easing value that accelerates from zero.
[[nodiscard]] POMDOG_EXPORT f32
easeInQuint(f32 x) noexcept;

/// Returns a quintic easing value that accelerates from zero.
[[nodiscard]] POMDOG_EXPORT f64
easeInQuint(f64 x) noexcept;

/// Returns a quintic easing value that decelerates to zero.
[[nodiscard]] POMDOG_EXPORT f32
easeOutQuint(f32 x) noexcept;

/// Returns a quintic easing value that decelerates to zero.
[[nodiscard]] POMDOG_EXPORT f64
easeOutQuint(f64 x) noexcept;

/// Returns a quintic easing value that accelerates then decelerates.
[[nodiscard]] POMDOG_EXPORT f32
easeInOutQuint(f32 x) noexcept;

/// Returns a quintic easing value that accelerates then decelerates.
[[nodiscard]] POMDOG_EXPORT f64
easeInOutQuint(f64 x) noexcept;

// MARK: Sinusoidal (Sine)

/// Returns a sinusoidal easing value that accelerates from zero.
[[nodiscard]] POMDOG_EXPORT f32
easeInSine(f32 x) noexcept;

/// Returns a sinusoidal easing value that accelerates from zero.
[[nodiscard]] POMDOG_EXPORT f64
easeInSine(f64 x) noexcept;

/// Returns a sinusoidal easing value that decelerates to zero.
[[nodiscard]] POMDOG_EXPORT f32
easeOutSine(f32 x) noexcept;

/// Returns a sinusoidal easing value that decelerates to zero.
[[nodiscard]] POMDOG_EXPORT f64
easeOutSine(f64 x) noexcept;

/// Returns a sinusoidal easing value that accelerates then decelerates.
[[nodiscard]] POMDOG_EXPORT f32
easeInOutSine(f32 x) noexcept;

/// Returns a sinusoidal easing value that accelerates then decelerates.
[[nodiscard]] POMDOG_EXPORT f64
easeInOutSine(f64 x) noexcept;

// MARK: Exponential (Expo)

/// Returns an exponential easing value that accelerates from zero.
[[nodiscard]] POMDOG_EXPORT f32
easeInExpo(f32 x) noexcept;

/// Returns an exponential easing value that accelerates from zero.
[[nodiscard]] POMDOG_EXPORT f64
easeInExpo(f64 x) noexcept;

/// Returns an exponential easing value that decelerates to zero.
[[nodiscard]] POMDOG_EXPORT f32
easeOutExpo(f32 x) noexcept;

/// Returns an exponential easing value that decelerates to zero.
[[nodiscard]] POMDOG_EXPORT f64
easeOutExpo(f64 x) noexcept;

/// Returns an exponential easing value that accelerates then decelerates.
[[nodiscard]] POMDOG_EXPORT f32
easeInOutExpo(f32 x) noexcept;

/// Returns an exponential easing value that accelerates then decelerates.
[[nodiscard]] POMDOG_EXPORT f64
easeInOutExpo(f64 x) noexcept;

// MARK: Circular (Circ)

/// Returns a circular easing value that accelerates from zero.
[[nodiscard]] POMDOG_EXPORT f32
easeInCirc(f32 x) noexcept;

/// Returns a circular easing value that accelerates from zero.
[[nodiscard]] POMDOG_EXPORT f64
easeInCirc(f64 x) noexcept;

/// Returns a circular easing value that decelerates to zero.
[[nodiscard]] POMDOG_EXPORT f32
easeOutCirc(f32 x) noexcept;

/// Returns a circular easing value that decelerates to zero.
[[nodiscard]] POMDOG_EXPORT f64
easeOutCirc(f64 x) noexcept;

/// Returns a circular easing value that accelerates then decelerates.
[[nodiscard]] POMDOG_EXPORT f32
easeInOutCirc(f32 x) noexcept;

/// Returns a circular easing value that accelerates then decelerates.
[[nodiscard]] POMDOG_EXPORT f64
easeInOutCirc(f64 x) noexcept;

// MARK: Elastic

/// Returns an elastic easing value that starts with spring-like oscillation.
///
/// The returned value may fall outside the `[0, 1]` range due to overshoot.
[[nodiscard]] POMDOG_EXPORT f32
easeInElastic(f32 x) noexcept;

/// Returns an elastic easing value that starts with spring-like oscillation.
///
/// The returned value may fall outside the `[0, 1]` range due to overshoot.
[[nodiscard]] POMDOG_EXPORT f64
easeInElastic(f64 x) noexcept;

/// Returns an elastic easing value that ends with spring-like oscillation.
///
/// The returned value may fall outside the `[0, 1]` range due to overshoot.
[[nodiscard]] POMDOG_EXPORT f32
easeOutElastic(f32 x) noexcept;

/// Returns an elastic easing value that ends with spring-like oscillation.
///
/// The returned value may fall outside the `[0, 1]` range due to overshoot.
[[nodiscard]] POMDOG_EXPORT f64
easeOutElastic(f64 x) noexcept;

/// Returns an elastic easing value with spring-like oscillation at both ends.
///
/// The returned value may fall outside the `[0, 1]` range due to overshoot.
[[nodiscard]] POMDOG_EXPORT f32
easeInOutElastic(f32 x) noexcept;

/// Returns an elastic easing value with spring-like oscillation at both ends.
///
/// The returned value may fall outside the `[0, 1]` range due to overshoot.
[[nodiscard]] POMDOG_EXPORT f64
easeInOutElastic(f64 x) noexcept;

// MARK: Bounce

/// Returns a bounce easing value that bounces at the start.
[[nodiscard]] POMDOG_EXPORT f32
easeInBounce(f32 x) noexcept;

/// Returns a bounce easing value that bounces at the start.
[[nodiscard]] POMDOG_EXPORT f64
easeInBounce(f64 x) noexcept;

/// Returns a bounce easing value that bounces at the end.
[[nodiscard]] POMDOG_EXPORT f32
easeOutBounce(f32 x) noexcept;

/// Returns a bounce easing value that bounces at the end.
[[nodiscard]] POMDOG_EXPORT f64
easeOutBounce(f64 x) noexcept;

/// Returns a bounce easing value that bounces at both ends.
[[nodiscard]] POMDOG_EXPORT f32
easeInOutBounce(f32 x) noexcept;

/// Returns a bounce easing value that bounces at both ends.
[[nodiscard]] POMDOG_EXPORT f64
easeInOutBounce(f64 x) noexcept;

// MARK: Back

/// Returns a back easing value that overshoots at the start before moving forward.
///
/// The returned value may fall outside the `[0, 1]` range due to overshoot.
[[nodiscard]] POMDOG_EXPORT f32
easeInBack(f32 x) noexcept;

/// Returns a back easing value that overshoots at the start before moving forward.
///
/// The returned value may fall outside the `[0, 1]` range due to overshoot.
[[nodiscard]] POMDOG_EXPORT f64
easeInBack(f64 x) noexcept;

/// Returns a back easing value that overshoots past the end before settling.
///
/// The returned value may fall outside the `[0, 1]` range due to overshoot.
[[nodiscard]] POMDOG_EXPORT f32
easeOutBack(f32 x) noexcept;

/// Returns a back easing value that overshoots past the end before settling.
///
/// The returned value may fall outside the `[0, 1]` range due to overshoot.
[[nodiscard]] POMDOG_EXPORT f64
easeOutBack(f64 x) noexcept;

/// Returns a back easing value that overshoots at both ends.
///
/// The returned value may fall outside the `[0, 1]` range due to overshoot.
[[nodiscard]] POMDOG_EXPORT f32
easeInOutBack(f32 x) noexcept;

/// Returns a back easing value that overshoots at both ends.
///
/// The returned value may fall outside the `[0, 1]` range due to overshoot.
[[nodiscard]] POMDOG_EXPORT f64
easeInOutBack(f64 x) noexcept;

} // namespace pomdog::math
