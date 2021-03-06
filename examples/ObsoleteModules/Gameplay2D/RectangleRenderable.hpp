// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Renderable.hpp"
#include "Pomdog.Experimental/Rendering/Commands/PrimitiveCommand.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {

class RectangleRenderable : public Renderable {
public:
    RectangleRenderable();

    void Visit(Entity& entity, Renderer& renderer) override;

    void LeftTopColor(const Pomdog::Color& color);
    void RightTopColor(const Pomdog::Color& color);
    void LeftBottomColor(const Pomdog::Color& color);
    void RightBottomColor(const Pomdog::Color& color);

    void OriginPivot(const Vector2& originPivot);
    Vector2 OriginPivot() const;

    void BoundingBox(const Rectangle& rectangle);
    Rectangle BoundingBox() const;

private:
    Detail::Rendering::PrimitiveCommand command;
    Vector2 originPivot;
    std::uint32_t dirtyFlags;
};

} // namespace Pomdog
