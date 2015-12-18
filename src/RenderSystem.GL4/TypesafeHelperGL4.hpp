// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include <type_traits>

namespace Pomdog {
namespace Detail {
namespace GL4 {

namespace TypesafeHelperGL4 {

template <class Tagged>
struct Traits;

template <class Tagged>
Tagged Get(Traits<Tagged>* = nullptr)
{
    typedef typename Tagged::value_type value_type;
    static_assert(std::is_fundamental<value_type>::value, "");
    static_assert(std::is_integral<value_type>::value, "");

    GLint buffer = 0;
    glGetIntegerv(Traits<Tagged>::BufferBinding, &buffer);
    return Tagged{ static_cast<value_type>(buffer) };
}

template <class Tagged>
void BindBuffer(Tagged const& buffer)
{
    glBindBuffer(Traits<Tagged>::BufferTarget, buffer.value);
}

template <class Tagged>
void BindTexture(Tagged const& texture)
{
    glBindTexture(Traits<Tagged>::TextureTarget, texture.value);
}

} // namespace TypesafeHelperGL4

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
