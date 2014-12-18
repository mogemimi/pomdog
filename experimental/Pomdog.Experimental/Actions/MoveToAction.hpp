//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_MOVETOACTION_69978A4F_8913_4A3E_86D4_AA5A69A58586_HPP
#define POMDOG_MOVETOACTION_69978A4F_8913_4A3E_86D4_AA5A69A58586_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "detail/TemporalAction.hpp"
#include "Pomdog.Experimental/Gameplay2D/Transform2D.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {
namespace Details {
namespace Actions {

class MoveTo {
private:
	Vector2 startPosition;
	Vector2 endPosition;

public:
	explicit MoveTo(Vector2 const& positionIn)
		: endPosition(positionIn)
	{}

	void Begin(GameObject const& gameObject)
	{
		POMDOG_ASSERT(gameObject);
		POMDOG_ASSERT(gameObject.HasComponent<Transform2D>());
		
		auto transform = gameObject.Component<Transform2D>();
		startPosition = transform->Position;
	}

	void Update(GameObject & gameObject, float normalizedTime)
	{
		POMDOG_ASSERT(gameObject);
		POMDOG_ASSERT(gameObject.HasComponent<Transform2D>());
		
		auto transform = gameObject.Component<Transform2D>();
		transform->Position = Vector2::Lerp(startPosition, endPosition, normalizedTime);
	}
};

}// namespace Actions
}// namespace Details

using MoveToAction = Details::Actions::TemporalAction<Details::Actions::MoveTo>;

}// namespace Pomdog

#endif // !defined(POMDOG_MOVETOACTION_69978A4F_8913_4A3E_86D4_AA5A69A58586_HPP)
