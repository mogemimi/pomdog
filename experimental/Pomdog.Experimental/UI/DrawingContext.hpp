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

    virtual void Push(const Matrix3x2& matrix) = 0;

    virtual void Pop() = 0;

    virtual void DrawRectangle(
        const Matrix3x2& transform,
        const Color& color,
        const Rectangle& rectangle) = 0;

    virtual void DrawLine(
        const Matrix3x2& transform,
        const Color& color,
        float penSize,
        const Vector2& point1,
        const Vector2& point2) = 0;

    virtual void DrawString(
        const Matrix3x2& transform,
        const Color& color,
        FontWeight fontWeight,
        FontSize fontSize,
        const std::string& text) = 0;

    //void DrawEllipse();

    //void DrawSprite();
};

} // namespace UI
} // namespace Pomdog
