// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Gameplay/Component.hpp"
#include "Pomdog.Experimental/Gameplay/GameObject.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"

namespace Pomdog {

class Renderer;

struct Material {
    Color Color = Color::White;
};

class Renderable: public Component<Renderable> {
public:
    virtual ~Renderable() = default;

    virtual void Visit(GameObject & gameObject, Renderer & renderer) = 0;

    Material Material;
    float DrawOrder = 0.0f;
    bool IsVisible = true;
};

}// namespace Pomdog
