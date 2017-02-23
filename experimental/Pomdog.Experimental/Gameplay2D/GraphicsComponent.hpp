// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Gameplay/detail/ComponentTypeIndex.hpp"
#include "Pomdog.Experimental/Gameplay/Component.hpp"
#include "Pomdog.Experimental/Gameplay/Entity.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"

namespace Pomdog {

class Renderer;

class GraphicsComponent : public Component {
public:
    virtual ~GraphicsComponent() = default;

    virtual void Visit(Entity & entity, Renderer & renderer) = 0;

    float GetDrawOrder() const noexcept;

    void SetDrawOrder(float drawOrderIn) noexcept;

    bool IsVisible() const noexcept;

    void SetVisible(bool isVisibleIn) noexcept;

private:
    float drawOrder = 0.0f;
    bool isVisible = true;
};

template <>
struct ComponentTypeDeclaration<GraphicsComponent> final {
    static std::uint8_t GetTypeIndex();
};

} // namespace Pomdog
