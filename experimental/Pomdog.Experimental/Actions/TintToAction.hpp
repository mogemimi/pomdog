// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TINTTOACTION_EC615DAE_A054_447B_88F8_E6FAF8AD0DB9_HPP
#define POMDOG_TINTTOACTION_EC615DAE_A054_447B_88F8_E6FAF8AD0DB9_HPP

#include "detail/TemporalAction.hpp"
#include "Pomdog.Experimental/Gameplay/GameObject.hpp"
#include "Pomdog.Experimental/Gameplay2D/Renderable.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
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
}// namespace Detail

using TintToAction = Detail::Actions::TemporalAction<Detail::Actions::TintTo>;

}// namespace Pomdog

#endif // !defined(POMDOG_TINTTOACTION_EC615DAE_A054_447B_88F8_E6FAF8AD0DB9_HPP)
