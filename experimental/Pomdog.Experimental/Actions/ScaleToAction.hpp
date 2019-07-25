// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "detail/TemporalAction.hpp"
#include "Pomdog.Experimental/Gameplay/Entity.hpp"
#include "Pomdog.Experimental/Gameplay2D/Transform.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace Actions {

class ScaleTo final {
private:
    Vector3 startScale;
    Vector3 endScale;

public:
    explicit ScaleTo(Vector3 const& scaleIn)
        : endScale(scaleIn)
    {}

    explicit ScaleTo(float scale)
        : endScale(scale, scale, scale)
    {}

    void Begin(Entity const& entity)
    {
        POMDOG_ASSERT(entity);
        POMDOG_ASSERT(entity.HasComponent<Transform>());

        auto transform = entity.GetComponent<Transform>();
        startScale = transform->GetScale();
    }

    void Update(Entity & entity, float normalizedTime)
    {
        POMDOG_ASSERT(entity);
        POMDOG_ASSERT(entity.HasComponent<Transform>());

        auto transform = entity.GetComponent<Transform>();
        transform->SetScale(Vector3::Lerp(startScale, endScale, normalizedTime));
    }
};

} // namespace Actions
} // namespace Detail

using ScaleToAction = Detail::Actions::TemporalAction<Detail::Actions::ScaleTo>;

} // namespace Pomdog
