// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SCALETOACTION_5EB92B4C_56E3_4D2B_AC93_999860ED5A8F_HPP
#define POMDOG_SCALETOACTION_5EB92B4C_56E3_4D2B_AC93_999860ED5A8F_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "detail/TemporalAction.hpp"
#include "Pomdog.Experimental/Gameplay/GameObject.hpp"
#include "Pomdog.Experimental/Gameplay2D/Transform2D.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace Actions {

class ScaleTo {
private:
    Vector2 startScale;
    Vector2 endScale;

public:
    explicit ScaleTo(Vector2 const& scaleIn)
        : endScale(scaleIn)
    {}

    void Begin(GameObject const& gameObject)
    {
        POMDOG_ASSERT(gameObject);
        POMDOG_ASSERT(gameObject.HasComponent<Transform2D>());

        auto transform = gameObject.Component<Transform2D>();
        startScale = transform->Scale;
    }

    void Update(GameObject & gameObject, float normalizedTime)
    {
        POMDOG_ASSERT(gameObject);
        POMDOG_ASSERT(gameObject.HasComponent<Transform2D>());

        auto transform = gameObject.Component<Transform2D>();
        transform->Scale = Vector2::Lerp(startScale, endScale, normalizedTime);
    }
};

}// namespace Actions
}// namespace Detail

using ScaleToAction = Detail::Actions::TemporalAction<Detail::Actions::ScaleTo>;

}// namespace Pomdog

#endif // !defined(POMDOG_SCALETOACTION_5EB92B4C_56E3_4D2B_AC93_999860ED5A8F_HPP)
