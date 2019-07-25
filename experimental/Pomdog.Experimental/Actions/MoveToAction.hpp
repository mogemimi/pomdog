// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "detail/TemporalAction.hpp"
#include "Pomdog.Experimental/Gameplay/Entity.hpp"
#include "Pomdog.Experimental/Gameplay2D/Transform.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace Actions {

class MoveTo final {
private:
    Vector2 startPosition;
    Vector2 endPosition;

public:
    explicit MoveTo(Vector2 const& positionIn)
        : endPosition(positionIn)
    {}

    void Begin(Entity const& entity)
    {
        POMDOG_ASSERT(entity);
        POMDOG_ASSERT(entity.HasComponent<Transform>());

        auto transform = entity.GetComponent<Transform>();
        startPosition = transform->GetPosition2D();
    }

    void Update(Entity & entity, float normalizedTime)
    {
        POMDOG_ASSERT(entity);
        POMDOG_ASSERT(entity.HasComponent<Transform>());

        auto transform = entity.GetComponent<Transform>();
        transform->SetPosition2D(Vector2::Lerp(startPosition, endPosition, normalizedTime));
    }
};

} // namespace Actions
} // namespace Detail

using MoveToAction = Detail::Actions::TemporalAction<Detail::Actions::MoveTo>;

} // namespace Pomdog
