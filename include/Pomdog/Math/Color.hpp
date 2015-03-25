// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_COLOR_E8ABB31A_72F7_4D75_A92F_A9E70658E957_HPP
#define POMDOG_COLOR_E8ABB31A_72F7_4D75_A92F_A9E70658E957_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <cfloat>

namespace Pomdog {

class POMDOG_EXPORT Color {
public:
    std::uint32_t PackedValue;

public:
    // Constructors:
    Color() = default;

    ///@brief Copy constructor.
    Color(Color const&) = default;

    ///@brief Move constructor.
    Color(Color &&) = default;

    ///@brief Construct from unsigned integer values.
    Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha);

    explicit Color(Vector3 const& vector);
    explicit Color(Vector4 const& vector);

    // Assignment operators:
    Color & operator=(Color const&) = default;
    Color & operator=(Color &&) = default;

    bool operator==(Color const&) const;
    bool operator!=(Color const&) const;

    std::uint8_t R() const;
    std::uint8_t G() const;
    std::uint8_t B() const;
    std::uint8_t A() const;

    void R(std::uint8_t value);
    void G(std::uint8_t value);
    void B(std::uint8_t value);
    void A(std::uint8_t value);

    Vector3 ToVector3() const;
    Vector4 ToVector4() const;

    static Color Lerp(Color const& source1, Color const& source2, float amount);

    static Color SmoothStep(Color const& source1, Color const& source2, float amount);

    // static public attributes

    ///@note (R, G, B, A) = (255, 255, 255, 255)
    static const Color White;

    ///@note (R, G, B, A) = (0, 0, 0, 255)
    static const Color Black;

    ///@note (R, G, B, A) = (255, 0, 0, 255)
    static const Color Red;

    ///@note (R, G, B, A) = (0, 255, 0, 255)
    static const Color Green;

    ///@note (R, G, B, A) = (0, 0, 255, 255)
    static const Color Blue;

    ///@note (R, G, B, A) = (255, 255, 0, 255)
    static const Color Yellow;

    ///@note (R, G, B, A) = (100, 149 ,237, 255)
    static const Color CornflowerBlue;

    ///@note (R, G, B, A) = (0, 0, 0, 0)
    static const Color TransparentBlack;
};

}// namespace Pomdog

#endif // !defined(POMDOG_COLOR_E8ABB31A_72F7_4D75_A92F_A9E70658E957_HPP)
