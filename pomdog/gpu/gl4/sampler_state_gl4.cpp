// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/sampler_state_gl4.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/gpu/gl4/format_helper.h"
#include "pomdog/gpu/sampler_descriptor.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::gl4 {
namespace {

GLenum ToTextureAddressMode(TextureAddressMode address) noexcept
{
    switch (address) {
    case TextureAddressMode::Wrap:
        return GL_REPEAT;
    case TextureAddressMode::Clamp:
        return GL_CLAMP_TO_EDGE;
    case TextureAddressMode::Mirror:
        return GL_MIRRORED_REPEAT;
    case TextureAddressMode::Border:
        return GL_CLAMP_TO_BORDER;
    }
    POMDOG_UNREACHABLE("Unsupported texture address mode");
}

} // namespace

std::unique_ptr<Error>
SamplerStateGL4::Initialize(const SamplerDescriptor& descriptor) noexcept
{
    samplerObject = ([] {
        SamplerObjectGL4 sampler;
        glGenSamplers(1, sampler.Data());
        POMDOG_CHECK_ERROR_GL4("glGenSamplers");
        return sampler;
    })();

    glSamplerParameteri(samplerObject->value, GL_TEXTURE_WRAP_S, ToTextureAddressMode(descriptor.AddressU));
    glSamplerParameteri(samplerObject->value, GL_TEXTURE_WRAP_T, ToTextureAddressMode(descriptor.AddressV));
    glSamplerParameteri(samplerObject->value, GL_TEXTURE_WRAP_R, ToTextureAddressMode(descriptor.AddressW));

    POMDOG_CHECK_ERROR_GL4("glSamplerParameteri");

    switch (descriptor.Filter) {
    case TextureFilter::Linear:
        glSamplerParameteri(samplerObject->value, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(samplerObject->value, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    case TextureFilter::Point:
        glSamplerParameteri(samplerObject->value, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glSamplerParameteri(samplerObject->value, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case TextureFilter::LinearMipPoint:
        glSamplerParameteri(samplerObject->value, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glSamplerParameteri(samplerObject->value, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    case TextureFilter::PointMipLinear:
        glSamplerParameteri(samplerObject->value, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glSamplerParameteri(samplerObject->value, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case TextureFilter::MinLinearMagPointMipLinear:
        glSamplerParameteri(samplerObject->value, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glSamplerParameteri(samplerObject->value, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case TextureFilter::MinLinearMagPointMipPoint:
        glSamplerParameteri(samplerObject->value, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glSamplerParameteri(samplerObject->value, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case TextureFilter::MinPointMagLinearMipLinear:
        glSamplerParameteri(samplerObject->value, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glSamplerParameteri(samplerObject->value, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    case TextureFilter::MinPointMagLinearMipPoint:
        glSamplerParameteri(samplerObject->value, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glSamplerParameteri(samplerObject->value, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    case TextureFilter::Anisotropic: {
        // FIXME: Not implemented
        glSamplerParameteri(samplerObject->value, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glSamplerParameteri(samplerObject->value, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        POMDOG_CHECK_ERROR_GL4("glSamplerParameteri");

        POMDOG_ASSERT(1 <= descriptor.MaxAnisotropy && descriptor.MaxAnisotropy <= 16);

        GLfloat deviceMaxAnisotropy = 1.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &deviceMaxAnisotropy);
        POMDOG_CHECK_ERROR_GL4("glGetFloatv");

        deviceMaxAnisotropy = std::min(deviceMaxAnisotropy,
            static_cast<decltype(deviceMaxAnisotropy)>(descriptor.MaxAnisotropy));

        glSamplerParameterf(samplerObject->value, GL_TEXTURE_MAX_ANISOTROPY_EXT, deviceMaxAnisotropy);
        POMDOG_CHECK_ERROR_GL4("glSamplerParameterf");
        break;
    }
    }

    {
        POMDOG_ASSERT(descriptor.MinMipLevel <= descriptor.MaxMipLevel);
        POMDOG_ASSERT(descriptor.MaxMipLevel <= std::numeric_limits<GLfloat>::max());

        glSamplerParameterf(samplerObject->value, GL_TEXTURE_MIN_LOD, descriptor.MinMipLevel);
        glSamplerParameterf(samplerObject->value, GL_TEXTURE_MAX_LOD, descriptor.MaxMipLevel);
        glSamplerParameterf(samplerObject->value, GL_TEXTURE_LOD_BIAS, descriptor.MipMapLevelOfDetailBias);
        POMDOG_CHECK_ERROR_GL4("glSamplerParameterf");
    }

    if (descriptor.ComparisonFunction == ComparisonFunction::Never) {
        glSamplerParameteri(samplerObject->value, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    }
    else {
        glSamplerParameteri(samplerObject->value, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glSamplerParameteri(
            samplerObject->value,
            GL_TEXTURE_COMPARE_FUNC,
            ToComparisonFunctionGL4NonTypesafe(descriptor.ComparisonFunction));
    }

    return nullptr;
}

SamplerStateGL4::~SamplerStateGL4()
{
    if (samplerObject) {
        glDeleteSamplers(1, samplerObject->Data());
        POMDOG_CHECK_ERROR_GL4("glDeleteSamplers");
    }
}

void SamplerStateGL4::Apply(int index)
{
    static_assert(GL_TEXTURE19 == (GL_TEXTURE0 + 19), "");
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index <= 19);

    POMDOG_ASSERT(samplerObject);
    glBindSampler(index, samplerObject->value);
    POMDOG_CHECK_ERROR_GL4("glBindSampler");
}

} // namespace pomdog::detail::gl4
