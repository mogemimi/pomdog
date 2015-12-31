// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "detail/TemporalAction.hpp"
#include "Pomdog.Experimental/Gameplay/GameObject.hpp"
#include "Pomdog.Experimental/Gameplay2D/Transform2D.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
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

} // namespace Actions
} // namespace Detail

using MoveToAction = Detail::Actions::TemporalAction<Detail::Actions::MoveTo>;

} // namespace Pomdog
