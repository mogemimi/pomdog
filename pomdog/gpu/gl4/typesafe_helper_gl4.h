// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {

namespace TypesafeHelperGL4 {

template <class Tagged>
struct Traits;

template <class Tagged>
Tagged Get(Traits<Tagged>* = nullptr)
{
    using value_type = typename Tagged::value_type;
    static_assert(std::is_fundamental<value_type>::value, "");
    static_assert(std::is_integral<value_type>::value, "");

    GLint buffer = 0;
    glGetIntegerv(Traits<Tagged>::BufferBinding, &buffer);
    return Tagged{static_cast<value_type>(buffer)};
}

template <class Tagged>
void BindBuffer(const Tagged& buffer)
{
    glBindBuffer(Traits<Tagged>::BufferTarget, buffer.value);
}

template <class Tagged>
void BindTexture(const Tagged& texture)
{
    glBindTexture(Traits<Tagged>::TextureTarget, texture.value);
}

} // namespace TypesafeHelperGL4

} // namespace pomdog::gpu::detail::gl4
