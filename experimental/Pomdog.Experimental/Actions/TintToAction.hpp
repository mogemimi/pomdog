// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "detail/TemporalAction.hpp"
#include "Pomdog.Experimental/Gameplay/Entity.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace Actions {

//class TintTo final {
//private:
//    Color startColor;
//    Color endColor;
//
//public:
//    explicit TintTo(Color const& colorIn)
//        : endColor(colorIn)
//    {}
//
//    void Begin(Entity const& entity)
//    {
//        POMDOG_ASSERT(entity);
//        POMDOG_ASSERT(entity.HasComponent<Renderable>());
//
//        auto renderable = entity.GetComponent<Renderable>();
//        startColor = renderable->Material.Color;
//    }
//
//    void Update(Entity & entity, float normalizedTime)
//    {
//        POMDOG_ASSERT(entity);
//        POMDOG_ASSERT(entity.HasComponent<Renderable>());
//
//        auto renderable = entity.GetComponent<Renderable>();
//        auto color = Color::Lerp(startColor, endColor, normalizedTime);
//        renderable->Material.Color = color;
//    }
//};

} // namespace Actions
} // namespace Detail

//using TintToAction = Detail::Actions::TemporalAction<Detail::Actions::TintTo>;

} // namespace Pomdog
