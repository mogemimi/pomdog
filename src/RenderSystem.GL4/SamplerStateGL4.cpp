// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "SamplerStateGL4.hpp"
#include "ErrorChecker.hpp"
#include "../Basic/Unreachable.hpp"
#include "Pomdog/Graphics/SamplerDescription.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>
#include <utility>

namespace Pomdog {
namespace Detail {
namespace GL4 {
namespace {

GLenum ToTextureAddressMode(TextureAddressMode address) noexcept
{
    switch (address) {
    case TextureAddressMode::Clamp: return GL_CLAMP_TO_EDGE;
    case TextureAddressMode::Mirror: return GL_MIRRORED_REPEAT;
    case TextureAddressMode::Wrap: return GL_REPEAT;
    }
    POMDOG_UNREACHABLE("Unsupported texture address mode");
}

} // unnamed namespace

SamplerStateGL4::SamplerStateGL4(const SamplerDescription& description)
{
    samplerObject = ([] {
        SamplerObjectGL4 sampler;
        glGenSamplers(1, sampler.Data());
        POMDOG_CHECK_ERROR_GL4("glGenSamplers");
        return sampler;
    })();

    glSamplerParameteri(samplerObject->value, GL_TEXTURE_WRAP_S, ToTextureAddressMode(description.AddressU));
    glSamplerParameteri(samplerObject->value, GL_TEXTURE_WRAP_T, ToTextureAddressMode(description.AddressV));
    glSamplerParameteri(samplerObject->value, GL_TEXTURE_WRAP_R, ToTextureAddressMode(description.AddressW));

    POMDOG_CHECK_ERROR_GL4("glSamplerParameteri");

    switch (description.Filter) {
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

        POMDOG_ASSERT(1 <= description.MaxAnisotropy && description.MaxAnisotropy <= 16);

        GLfloat deviceMaxAnisotropy = 1.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &deviceMaxAnisotropy);
        POMDOG_CHECK_ERROR_GL4("glGetFloatv");

        deviceMaxAnisotropy = std::min(deviceMaxAnisotropy,
            static_cast<decltype(deviceMaxAnisotropy)>(description.MaxAnisotropy));

        glSamplerParameterf(samplerObject->value, GL_TEXTURE_MAX_ANISOTROPY_EXT, deviceMaxAnisotropy);
        POMDOG_CHECK_ERROR_GL4("glSamplerParameterf");
        break;
    }
    }

    {
        POMDOG_ASSERT(description.MinMipLevel <= description.MaxMipLevel);
        POMDOG_ASSERT(description.MaxMipLevel <= std::numeric_limits<GLfloat>::max());

        glSamplerParameterf(samplerObject->value, GL_TEXTURE_MIN_LOD, description.MaxMipLevel);
        glSamplerParameterf(samplerObject->value, GL_TEXTURE_MAX_LOD, description.MaxMipLevel);
        glSamplerParameterf(samplerObject->value, GL_TEXTURE_LOD_BIAS, description.MipMapLevelOfDetailBias);
        POMDOG_CHECK_ERROR_GL4("glSamplerParameterf");
    }
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

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
