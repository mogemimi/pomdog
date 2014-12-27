//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_TINTTOACTION_EC615DAE_A054_447B_88F8_E6FAF8AD0DB9_HPP
#define POMDOG_TINTTOACTION_EC615DAE_A054_447B_88F8_E6FAF8AD0DB9_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "detail/TemporalAction.hpp"
#include "Pomdog.Experimental/Gameplay2D/Renderable.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Gameplay/GameObject.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Details {
namespace Actions {

class TintTo {
private:
	Color startColor;
	Color endColor;

public:
	explicit TintTo(Color const& colorIn)
		: endColor(colorIn)
	{}

	void Begin(GameObject const& gameObject)
	{
		POMDOG_ASSERT(gameObject);
		POMDOG_ASSERT(gameObject.HasComponent<Renderable>());
		
		auto renderable = gameObject.Component<Renderable>();
		startColor = renderable->Material.Color;
	}

	void Update(GameObject & gameObject, float normalizedTime)
	{
		POMDOG_ASSERT(gameObject);
		POMDOG_ASSERT(gameObject.HasComponent<Renderable>());
		
		auto renderable = gameObject.Component<Renderable>();
		auto color = Color::Lerp(startColor, endColor, normalizedTime);
		renderable->Material.Color = color;
	}
};

}// namespace Actions
}// namespace Details

using TintToAction = Details::Actions::TemporalAction<Details::Actions::TintTo>;

}// namespace Pomdog

#endif // !defined(POMDOG_TINTTOACTION_EC615DAE_A054_447B_88F8_E6FAF8AD0DB9_HPP)
