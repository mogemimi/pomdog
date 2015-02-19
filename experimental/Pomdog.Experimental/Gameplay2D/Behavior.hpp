//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_BEHAVIOR_00BB3FAF_A505_418B_A5B0_937A692B91E8_HPP
#define POMDOG_BEHAVIOR_00BB3FAF_A505_418B_A5B0_937A692B91E8_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog.Experimental/Gameplay/Component.hpp"
#include "Pomdog/Application/Duration.hpp"
#include <memory>

namespace Pomdog {

class GameObject;

class Behavior: public Component<Behavior> {
public:
	virtual ~Behavior() = default;

	virtual void Start(GameObject & gameObject) = 0;

	virtual void Update(GameObject & gameObject, Duration const& frameDuration) = 0;
};

}// namespace Pomdog

#endif // !defined(POMDOG_BEHAVIOR_00BB3FAF_A505_418B_A5B0_937A692B91E8_HPP)
