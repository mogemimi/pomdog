// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_CONTAINMENTTYPE_21BFFFDA_D1CD_4222_B556_0EFF89954D5D_HPP
#define POMDOG_CONTAINMENTTYPE_21BFFFDA_D1CD_4222_B556_0EFF89954D5D_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

enum class ContainmentType: std::uint8_t {
    Contains,

    Disjoint,

    Intersects
};

}// namespace Pomdog

#endif // !defined(POMDOG_CONTAINMENTTYPE_21BFFFDA_D1CD_4222_B556_0EFF89954D5D_HPP)
