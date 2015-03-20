// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_ACTION_F54B5D22_1554_43AF_8F4A_F79479F7F312_HPP
#define POMDOG_ACTION_F54B5D22_1554_43AF_8F4A_F79479F7F312_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog.Experimental/Skeletal2D/AnimationTimeInterval.hpp"

namespace Pomdog {

class GameObject;

class Action {
public:
	virtual ~Action() = default;

	virtual void Act(GameObject & gameObject, AnimationTimeInterval const& duration) = 0;

	virtual bool IsCompleted() const = 0;
};

}// namespace Pomdog

#endif // !defined(POMDOG_ACTION_F54B5D22_1554_43AF_8F4A_F79479F7F312_HPP)
