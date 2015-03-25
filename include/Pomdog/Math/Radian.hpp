// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_RADIAN_671670CD_1B1C_4719_B0A1_D666BA07A0A9_HPP
#define POMDOG_RADIAN_671670CD_1B1C_4719_B0A1_D666BA07A0A9_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "detail/TaggedArithmetic.hpp"

namespace Pomdog {
namespace Detail {
namespace Tags {

struct RadianTag {};

}// namespace Tags
}// namespace Detail

template <typename T>
using Radian = Detail::TaggedArithmetic<T, Detail::Tags::RadianTag>;

}// namespace Pomdog

#endif // !defined(POMDOG_RADIAN_671670CD_1B1C_4719_B0A1_D666BA07A0A9_HPP)
