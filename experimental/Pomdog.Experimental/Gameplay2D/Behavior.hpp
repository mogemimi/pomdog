//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_BEHAVIOR_00BB3FAF_A505_418B_A5B0_937A692B91E8_HPP
#define POMDOG_BEHAVIOR_00BB3FAF_A505_418B_A5B0_937A692B91E8_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <Pomdog/Application/DurationSeconds.hpp>
#include <Pomdog/Gameplay/Component.hpp>
#include <memory>

namespace Pomdog {

class GameObject;

class Behavior: public Component<Behavior> {
public:
	virtual ~Behavior() = default;

	virtual void Start(GameObject & gameObject) = 0;
	
	virtual void Update(GameObject & gameObject, DurationSeconds const& frameDuration) = 0;
};

}// namespace Pomdog

#endif // !defined(POMDOG_BEHAVIOR_00BB3FAF_A505_418B_A5B0_937A692B91E8_HPP)
