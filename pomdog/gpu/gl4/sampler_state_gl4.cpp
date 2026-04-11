// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/sampler_state_gl4.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/gpu/gl4/format_helper.h"
#include "pomdog/gpu/sampler_desc.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {
namespace {

[[nodiscard]] GLenum
toTextureAddressMode(TextureAddressMode address) noexcept
{
    switch (address) {
    case TextureAddressMode::Wrap:
        return GL_REPEAT;
    case TextureAddressMode::Clamp:
        return GL_CLAMP_TO_EDGE;
    case TextureAddressMode::Mirror:
        return GL_MIRRORED_REPEAT;
    case TextureAddressMode::Border:
#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
        // NOTE: GL_CLAMP_TO_BORDER is not supported in WebGL 2.0 (OpenGL ES 3.0)
        return GL_CLAMP_TO_EDGE;
#else
        return GL_CLAMP_TO_BORDER;
#endif
    }
    POMDOG_UNREACHABLE("Unsupported texture address mode");
}

} // namespace

std::unique_ptr<Error>
SamplerStateGL4::initialize(const SamplerDesc& descriptor) noexcept
{
    samplerObject_ = ([] {
        SamplerObjectGL4 sampler;
        glGenSamplers(1, sampler.Data());
        POMDOG_CHECK_ERROR_GL4("glGenSamplers");
        return sampler;
    })();

    glSamplerParameteri(samplerObject_->value, GL_TEXTURE_WRAP_S, toTextureAddressMode(descriptor.addressU));
    glSamplerParameteri(samplerObject_->value, GL_TEXTURE_WRAP_T, toTextureAddressMode(descriptor.addressV));
    glSamplerParameteri(samplerObject_->value, GL_TEXTURE_WRAP_R, toTextureAddressMode(descriptor.addressW));

    POMDOG_CHECK_ERROR_GL4("glSamplerParameteri");

    switch (descriptor.filter) {
    case TextureFilter::Linear:
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    case TextureFilter::Point:
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case TextureFilter::LinearMipPoint:
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    case TextureFilter::PointMipLinear:
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case TextureFilter::MinLinearMagPointMipLinear:
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case TextureFilter::MinLinearMagPointMipPoint:
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case TextureFilter::MinPointMagLinearMipLinear:
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    case TextureFilter::MinPointMagLinearMipPoint:
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    case TextureFilter::Anisotropic: {
#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
        // NOTE: Anisotropic filtering is not supported in WebGL 2.0 core.
        // Fall back to linear mipmap filtering.
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        POMDOG_CHECK_ERROR_GL4("glSamplerParameteri");
#else
        // FIXME: Not implemented
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        POMDOG_CHECK_ERROR_GL4("glSamplerParameteri");

        POMDOG_ASSERT(1 <= descriptor.maxAnisotropy && descriptor.maxAnisotropy <= 16);

        GLfloat deviceMaxAnisotropy = 1.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &deviceMaxAnisotropy);
        POMDOG_CHECK_ERROR_GL4("glGetFloatv");

        deviceMaxAnisotropy = std::min(deviceMaxAnisotropy,
            static_cast<decltype(deviceMaxAnisotropy)>(descriptor.maxAnisotropy));

        glSamplerParameterf(samplerObject_->value, GL_TEXTURE_MAX_ANISOTROPY_EXT, deviceMaxAnisotropy);
        POMDOG_CHECK_ERROR_GL4("glSamplerParameterf");
#endif
        break;
    }
    }

    {
        POMDOG_ASSERT(descriptor.minMipLevel <= descriptor.maxMipLevel);
        POMDOG_ASSERT(descriptor.maxMipLevel <= std::numeric_limits<GLfloat>::max());

        glSamplerParameterf(samplerObject_->value, GL_TEXTURE_MIN_LOD, descriptor.minMipLevel);
        glSamplerParameterf(samplerObject_->value, GL_TEXTURE_MAX_LOD, descriptor.maxMipLevel);
#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
        // NOTE: GL_TEXTURE_LOD_BIAS is not supported in WebGL 2.0 (OpenGL ES 3.0)
        glSamplerParameterf(samplerObject_->value, GL_TEXTURE_LOD_BIAS, descriptor.mipmapLevelOfDetailBias);
#endif
        POMDOG_CHECK_ERROR_GL4("glSamplerParameterf");
    }

    if (descriptor.comparisonFunction == ComparisonFunction::Never) {
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    }
    else {
        glSamplerParameteri(samplerObject_->value, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glSamplerParameteri(
            samplerObject_->value,
            GL_TEXTURE_COMPARE_FUNC,
            toComparisonFunctionGL4NonTypesafe(descriptor.comparisonFunction));
    }

    return nullptr;
}

SamplerStateGL4::~SamplerStateGL4()
{
    if (samplerObject_) {
        glDeleteSamplers(1, samplerObject_->Data());
        POMDOG_CHECK_ERROR_GL4("glDeleteSamplers");
    }
}

void SamplerStateGL4::apply(int index)
{
    static_assert(GL_TEXTURE19 == (GL_TEXTURE0 + 19), "");
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index <= 19);

    POMDOG_ASSERT(samplerObject_);
    glBindSampler(index, samplerObject_->value);
    POMDOG_CHECK_ERROR_GL4("glBindSampler");
}

} // namespace pomdog::gpu::detail::gl4
