// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_CLEAROPTIONS_F51CCEF3_HPP
#define POMDOG_CLEAROPTIONS_F51CCEF3_HPP

#include "Pomdog/Basic/Export.hpp"
#include <cstdint>

namespace Pomdog {

enum class ClearOptions: std::uint8_t {
    ///@brief Clear the depth buffer.
    DepthBuffer = 0b001,

    ///@brief Clear the stencil buffer.
    Stencil = 0b010,

    ///@brief Clear the rendering target.
    RenderTarget = 0b100,
};

constexpr ClearOptions operator|(ClearOptions a, ClearOptions b) noexcept
{
    return static_cast<ClearOptions>(
        static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b));
}

#if __cplusplus < 201402L
POMDOG_EXPORT ClearOptions & operator|=(ClearOptions & a, ClearOptions b) noexcept;
#else
constexpr ClearOptions & operator|=(ClearOptions & a, ClearOptions b) noexcept
{
    a = a | b;
    return a;
}
#endif

} // namespace Pomdog

#endif // POMDOG_CLEAROPTIONS_F51CCEF3_HPP
