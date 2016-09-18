// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "detail/TemporalAction.hpp"
#include "Pomdog.Experimental/Gameplay/Entity.hpp"
#include "Pomdog.Experimental/Gameplay2D/Transform2D.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace Actions {

class ScaleTo final {
private:
    Vector2 startScale;
    Vector2 endScale;

public:
    explicit ScaleTo(Vector2 const& scaleIn)
        : endScale(scaleIn)
    {}

    void Begin(Entity const& entity)
    {
        POMDOG_ASSERT(entity);
        POMDOG_ASSERT(entity.HasComponent<Transform2D>());

        auto transform = entity.GetComponent<Transform2D>();
        startScale = transform->Scale;
    }

    void Update(Entity & entity, float normalizedTime)
    {
        POMDOG_ASSERT(entity);
        POMDOG_ASSERT(entity.HasComponent<Transform2D>());

        auto transform = entity.GetComponent<Transform2D>();
        transform->Scale = Vector2::Lerp(startScale, endScale, normalizedTime);
    }
};

} // namespace Actions
} // namespace Detail

using ScaleToAction = Detail::Actions::TemporalAction<Detail::Actions::ScaleTo>;

} // namespace Pomdog
