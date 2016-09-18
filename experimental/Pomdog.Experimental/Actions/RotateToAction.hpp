// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "detail/TemporalAction.hpp"
#include "Pomdog.Experimental/Gameplay/Entity.hpp"
#include "Pomdog.Experimental/Gameplay2D/Transform.hpp"
#include "Pomdog/Math/Quaternion.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace Actions {

class RotateTo final {
private:
    Vector3 startRotation;
    Vector3 endRotation;

public:
    explicit RotateTo(Quaternion const& rotation)
        : endRotation(Vector3::ToEulerAngles(rotation))
    {}

    RotateTo(const Radian<float>& rotateX, const Radian<float>& rotateY, const Radian<float>& rotateZ)
        : endRotation(rotateX.value, rotateY.value, rotateZ.value)
    {}

    void Begin(Entity const& entity)
    {
        POMDOG_ASSERT(entity);
        POMDOG_ASSERT(entity.HasComponent<Transform>());

        auto transform = entity.GetComponent<Transform>();
        startRotation = Vector3::ToEulerAngles(transform->GetRotation());
    }

    void Update(Entity & entity, float normalizedTime)
    {
        POMDOG_ASSERT(entity);
        POMDOG_ASSERT(entity.HasComponent<Transform>());

        auto transform = entity.GetComponent<Transform>();
        auto eularAngles = Vector3::Lerp(startRotation, endRotation, normalizedTime);
        transform->SetRotation(Quaternion::CreateFromYawPitchRoll(
            eularAngles.Y, eularAngles.X, eularAngles.Z));
    }
};

} // namespace Actions
} // namespace Detail

using RotateToAction = Detail::Actions::TemporalAction<Detail::Actions::RotateTo>;

} // namespace Pomdog
