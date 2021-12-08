// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::random {

class Xoroshiro128StarStar final {
private:
    std::uint64_t s[2];

public:
    using result_type = std::uint64_t;

    Xoroshiro128StarStar() noexcept;

    explicit Xoroshiro128StarStar(std::uint64_t seed) noexcept;

    std::uint64_t Next() noexcept;

    void Jump() noexcept;

    std::uint64_t operator()() noexcept;

    static constexpr std::uint64_t min() noexcept
    {
        return std::numeric_limits<std::uint64_t>::min();
    }

    static constexpr std::uint64_t max() noexcept
    {
        return std::numeric_limits<std::uint64_t>::max();
    }
};

} // namespace pomdog::random
