// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_RECTANGLERENDERABLE_FFA98981_HPP
#define POMDOG_RECTANGLERENDERABLE_FFA98981_HPP

#include "Renderable.hpp"
#include "Pomdog.Experimental/Rendering/Commands/PrimitiveCommand.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {

class RectangleRenderable: public Renderable {
public:
    RectangleRenderable();

    void Visit(GameObject & gameObject, Renderer & renderer) override;

    void LeftTopColor(Pomdog::Color const& color);
    void RightTopColor(Pomdog::Color const& color);
    void LeftBottomColor(Pomdog::Color const& color);
    void RightBottomColor(Pomdog::Color const& color);

    void OriginPivot(Vector2 const& originPivot);
    Vector2 OriginPivot() const;

    void BoundingBox(Rectangle const& rectangle);
    Rectangle BoundingBox() const;


private:
    Detail::Rendering::PrimitiveCommand command;
    Vector2 originPivot;
    std::uint32_t dirtyFlags;
};

}// namespace Pomdog

#endif // POMDOG_RECTANGLERENDERABLE_FFA98981_HPP
