// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::random {

/// Represents a xoroshiro128** pseudo-random number generator.
class Xoroshiro128StarStar final {
private:
    u64 s[2];

public:
    using result_type = u64;

    /// Creates a new Xoroshiro128StarStar with a default seed.
    Xoroshiro128StarStar() noexcept;

    /// Creates a new Xoroshiro128StarStar with the specified seed.
    explicit Xoroshiro128StarStar(u64 seed) noexcept;

    /// Generates the next random number.
    u64
    next() noexcept;

    /// Advances the state as if 2^64 calls to next() were made.
    void
    jump() noexcept;

    /// Generates the next random number.
    [[nodiscard]] u64
    operator()() noexcept;

    /// Preserves the internal state.
    [[nodiscard]] std::tuple<u64, u64>
    preserve() const noexcept;

    /// Reconstitutes the internal state.
    void
    reconstitute(u64 s0, u64 s1) noexcept;

    /// Returns the minimum value that can be generated.
    [[nodiscard]] static constexpr u64
    min() noexcept
    {
        return std::numeric_limits<u64>::min();
    }

    /// Returns the maximum value that can be generated.
    [[nodiscard]] static constexpr u64
    max() noexcept
    {
        return std::numeric_limits<u64>::max();
    }
};

} // namespace pomdog::random
