//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_GAMEOBJECTID_F68B7FC0_7009_433E_8F5E_153D5502766A_HPP
#define POMDOG_GAMEOBJECTID_F68B7FC0_7009_433E_8F5E_153D5502766A_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>
#include "../Utility/detail/Tagged.hpp"

namespace Pomdog {
namespace Details {
namespace GamePlay {

namespace Tags {

struct GameObjectIDTag {};

}// namespace Tags

}// namespace GamePlay
}// namespace Details

/// @addtogroup Framework
/// @{
/// @addtogroup GamePlay
/// @{

using GameObjectID = Details::Tagged<std::uint64_t, Details::GamePlay::Tags::GameObjectIDTag>;

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEOBJECTID_F68B7FC0_7009_433E_8F5E_153D5502766A_HPP)
