// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include <cstdint>
#include <string>

namespace Pomdog {
namespace UI {

enum class FontWeight : std::uint8_t {
    Light,
    Normal,
    Bold,
};

enum class FontSize : std::uint8_t {
    Small,
    Medium,
    Large,
};

class DrawingContext {
public:
    virtual ~DrawingContext() = default;

    virtual Matrix3x2 Top() const = 0;

    virtual void Push(Matrix3x2 const& matrix) = 0;

    virtual void Pop() = 0;

    virtual void DrawRectangle(
        Matrix3x2 const& transform,
        Color const& color,
        Rectangle const& rectangle) = 0;

    virtual void DrawLine(
        Matrix3x2 const& transform,
        Color const& color,
        float penSize,
        Vector2 const& point1,
        Vector2 const& point2) = 0;

    virtual void DrawString(
        Matrix3x2 const& transform,
        Color const& color,
        FontWeight fontWeight,
        FontSize fontSize,
        std::string const& text) = 0;

    //void DrawEllipse();

    //void DrawSprite();
};

} // namespace UI
} // namespace Pomdog
