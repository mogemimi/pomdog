// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Actions/Action.hpp"
#include "Pomdog.Experimental/Gameplay/Component.hpp"
#include "Pomdog.Experimental/Gameplay/Entity.hpp"
#include <algorithm>
#include <utility>
#include <vector>

namespace Pomdog {

class ActorComponent final : public Component {
public:
    void RunAction(std::unique_ptr<Action>&& action);

    void Act(Entity& entity, const Skeletal2D::AnimationTimeInterval& frameDuration);

    void StopActions();

private:
    std::vector<std::unique_ptr<Action>> actions;
};

template <>
struct ComponentTypeDeclaration<ActorComponent> final {
    static std::uint8_t GetTypeIndex();
};

template <>
class ComponentCreator<ActorComponent> final : public ComponentCreatorBase {
public:
    std::shared_ptr<Component> CreateComponent() override;

    std::uint8_t GetComponentType() override;
};

} // namespace Pomdog
