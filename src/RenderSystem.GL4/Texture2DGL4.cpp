// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Texture2DGL4.hpp"
#include "ErrorChecker.hpp"
#include "TypesafeHelperGL4.hpp"
#include "../Basic/Unreachable.hpp"
#include "../RenderSystem/SurfaceFormatHelper.hpp"
#include "../Utility/ScopeGuard.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>

namespace Pomdog {
namespace Detail {
namespace GL4 {
namespace {

GLenum ToInternalFormatGL4(SurfaceFormat format) noexcept
{
    switch (format) {
    case SurfaceFormat::BlockComp1_UNorm:
        return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    case SurfaceFormat::BlockComp2_UNorm:
        return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    case SurfaceFormat::BlockComp3_UNorm:
        return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    case SurfaceFormat::R8_UNorm:
        return GL_R8;
    case SurfaceFormat::R8G8_UNorm:
        return GL_RG8;
    case SurfaceFormat::R8G8B8A8_UNorm:
    case SurfaceFormat::B8G8R8A8_UNorm:
        return GL_RGBA8;
    case SurfaceFormat::R11G11B10_Float:
        return GL_R11F_G11F_B10F;
    case SurfaceFormat::R32G32B32A32_Float:
        return GL_RGBA32F;
    case SurfaceFormat::R10G10B10A2_UNorm:
        return GL_RGB10_A2;
    case SurfaceFormat::R16G16_Float:
        return GL_RG16;
    case SurfaceFormat::R16G16B16A16_Float:
        return GL_RGBA16;
    case SurfaceFormat::R32_Float:
        return GL_R32F;
    case SurfaceFormat::A8_UNorm:
        return GL_R8;
    }
    POMDOG_UNREACHABLE("Unsupported surface format");
}

GLenum ToFormatComponents(SurfaceFormat format) noexcept
{
    switch (format) {
    case SurfaceFormat::R8G8B8A8_UNorm:
    case SurfaceFormat::R16G16B16A16_Float:
    case SurfaceFormat::R32G32B32A32_Float:
    case SurfaceFormat::R10G10B10A2_UNorm:
        return GL_RGBA;
    case SurfaceFormat::R11G11B10_Float:
        return GL_RGB;
    case SurfaceFormat::R8G8_UNorm:
    case SurfaceFormat::R16G16_Float:
        return GL_RG;
    case SurfaceFormat::R8_UNorm:
    case SurfaceFormat::R32_Float:
    case SurfaceFormat::A8_UNorm:
        return GL_RED;
    case SurfaceFormat::B8G8R8A8_UNorm:
        return GL_BGRA;
    case SurfaceFormat::BlockComp1_UNorm:
    case SurfaceFormat::BlockComp2_UNorm:
    case SurfaceFormat::BlockComp3_UNorm:
        // Cannot find format
        return GL_RED;
    }
    POMDOG_UNREACHABLE("Unsupported surface format");
}

GLenum ToPixelFundamentalType(SurfaceFormat format) noexcept
{
    switch (format) {
    case SurfaceFormat::A8_UNorm:
    case SurfaceFormat::R8_UNorm:
    case SurfaceFormat::R8G8_UNorm:
    case SurfaceFormat::R8G8B8A8_UNorm:
    case SurfaceFormat::B8G8R8A8_UNorm:
        return GL_UNSIGNED_BYTE;
    case SurfaceFormat::R11G11B10_Float:
    case SurfaceFormat::R32G32B32A32_Float:
    case SurfaceFormat::R16G16_Float:
    case SurfaceFormat::R16G16B16A16_Float:
    case SurfaceFormat::R32_Float:
        return GL_FLOAT;
        break;
    case SurfaceFormat::R10G10B10A2_UNorm:
        return GL_UNSIGNED_INT_10_10_10_2;
        break;
    case SurfaceFormat::BlockComp1_UNorm:
    case SurfaceFormat::BlockComp2_UNorm:
    case SurfaceFormat::BlockComp3_UNorm:
        // Cannot find format
        return GL_UNSIGNED_BYTE;
    }
    POMDOG_UNREACHABLE("Unsupported surface format");
}

GLsizei MipmapImageDataBytes(
    GLsizei width,
    GLsizei height,
    GLsizei bytesPerBlock) noexcept
{
    return width * height * bytesPerBlock;
}

void SetPixelDataTexture2DCompressedGL4(
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    SurfaceFormat format,
    const void* pixelData)
{
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 1);
    POMDOG_ASSERT(pixelData != nullptr);
    POMDOG_ASSERT(format == SurfaceFormat::BlockComp1_UNorm
        || format == SurfaceFormat::BlockComp2_UNorm
        || format == SurfaceFormat::BlockComp3_UNorm);

    auto const internalFormat = ToInternalFormatGL4(format);
    auto const bytesPerBlock = SurfaceFormatHelper::ToBytesPerBlock(format);
    std::size_t startOffset = 0;

    GLsizei mipMapPixelWidth = pixelWidth;
    GLsizei mipMapPixelHeight = pixelHeight;

    for (GLint mipmapLevel = 0; mipmapLevel < levelCount; ++mipmapLevel) {
        POMDOG_ASSERT(mipMapPixelWidth > 0);
        POMDOG_ASSERT(mipMapPixelHeight > 0);

        GLsizei const strideBytesPerMipmap = MipmapImageDataBytes(
            mipMapPixelWidth, mipMapPixelHeight, bytesPerBlock);

        glCompressedTexSubImage2D(GL_TEXTURE_2D,
            mipmapLevel, 0, 0,
            mipMapPixelWidth,
            mipMapPixelHeight,
            internalFormat,
            strideBytesPerMipmap,
            reinterpret_cast<const std::uint8_t*>(pixelData) + startOffset);
        POMDOG_CHECK_ERROR_GL4("glCompressedTexSubImage2D");

        startOffset += strideBytesPerMipmap;

        mipMapPixelWidth = std::max((mipMapPixelWidth >> 1), 1);
        mipMapPixelHeight = std::max((mipMapPixelHeight >> 1), 1);
    }
}

void SetPixelDataTexture2DGL4(
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    SurfaceFormat format,
    const void* pixelData)
{
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 1);
    POMDOG_ASSERT(pixelData != nullptr);
    POMDOG_ASSERT(format != SurfaceFormat::BlockComp1_UNorm
        && format != SurfaceFormat::BlockComp2_UNorm
        && format != SurfaceFormat::BlockComp3_UNorm);

    auto const formatComponents = ToFormatComponents(format);
    auto const pixelFundamentalType = ToPixelFundamentalType(format);
    auto const bytesPerBlock = SurfaceFormatHelper::ToBytesPerBlock(format);

    GLsizei mipMapPixelWidth = pixelWidth;
    GLsizei mipMapPixelHeight = pixelHeight;

    std::size_t startOffset = 0;

    for (GLint mipmapLevel = 0; mipmapLevel < levelCount; ++mipmapLevel) {
        GLsizei const strideBytesPerMipmap = MipmapImageDataBytes(
            mipMapPixelWidth, mipMapPixelHeight, bytesPerBlock);

        glTexSubImage2D(GL_TEXTURE_2D,
            mipmapLevel, 0, 0,
            mipMapPixelWidth,
            mipMapPixelHeight,
            formatComponents,
            pixelFundamentalType,
            reinterpret_cast<const std::uint8_t*>(pixelData) + startOffset);
        POMDOG_CHECK_ERROR_GL4("glTexSubImage2D");

        startOffset += strideBytesPerMipmap;

        mipMapPixelWidth = std::max((mipMapPixelWidth >> 1), 1);
        mipMapPixelHeight = std::max((mipMapPixelHeight >> 1), 1);
    }
}

} // unnamed namespace

template<> struct TypesafeHelperGL4::Traits<Texture2DObjectGL4> {
    constexpr static GLenum BufferBinding = GL_TEXTURE_BINDING_2D;
    constexpr static GLenum TextureTarget = GL_TEXTURE_2D;
};

Texture2DGL4::Texture2DGL4(std::int32_t pixelWidth, std::int32_t pixelHeight,
    std::int32_t levelCount, SurfaceFormat format)
{
    // Create Texture2D
    textureObject = ([] {
        Texture2DObjectGL4 nativeTexture;
        glGenTextures(1, nativeTexture.Data());
        return nativeTexture;
    })();

    auto const prevTexture = TypesafeHelperGL4::Get<Texture2DObjectGL4>();
    ScopeGuard scope([&prevTexture] {
        TypesafeHelperGL4::BindTexture(prevTexture);
    });

    POMDOG_ASSERT(textureObject);
    TypesafeHelperGL4::BindTexture(*textureObject);
    POMDOG_CHECK_ERROR_GL4("glBindTexture");

    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 1);
    POMDOG_ASSERT(levelCount <= std::numeric_limits<GLsizei>::max());

    glTexStorage2D(GL_TEXTURE_2D, levelCount, ToInternalFormatGL4(format),
        pixelWidth, pixelHeight);
    POMDOG_CHECK_ERROR_GL4("glTexStorage2D");

    // Set mipmap levels
    POMDOG_ASSERT(levelCount >= 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, levelCount - 1);

    if (format == SurfaceFormat::A8_UNorm) {
        // NOTE: Emulate DXGI_FORMAT_A8_UNORM or MTLPixelFormatA8Unorm on OpenGL.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_GREEN);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);
    }
}

Texture2DGL4::~Texture2DGL4()
{
    if (textureObject) {
        glDeleteTextures(1, textureObject->Data());
        POMDOG_CHECK_ERROR_GL4("glDeleteTextures");
    }
}

void Texture2DGL4::SetData(std::int32_t pixelWidth, std::int32_t pixelHeight,
    std::int32_t levelCount, SurfaceFormat format, const void* pixelData)
{
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 1);
    POMDOG_ASSERT(pixelData != nullptr);

    const auto oldTexture = TypesafeHelperGL4::Get<Texture2DObjectGL4>();
    ScopeGuard scope([&] { TypesafeHelperGL4::BindTexture(oldTexture); });

    POMDOG_ASSERT(textureObject);
    TypesafeHelperGL4::BindTexture(*textureObject);
    POMDOG_CHECK_ERROR_GL4("glBindTexture");

    switch (format) {
    case SurfaceFormat::BlockComp1_UNorm:
    case SurfaceFormat::BlockComp2_UNorm:
    case SurfaceFormat::BlockComp3_UNorm:
        SetPixelDataTexture2DCompressedGL4(
            pixelWidth, pixelHeight, levelCount, format, pixelData);
        break;
    default:
        SetPixelDataTexture2DGL4(
            pixelWidth, pixelHeight, levelCount, format, pixelData);
        break;
    }
}

void Texture2DGL4::GenerateMipmap()
{
    POMDOG_ASSERT(textureObject);

    const auto oldTexture = TypesafeHelperGL4::Get<Texture2DObjectGL4>();
    ScopeGuard scope([&] { TypesafeHelperGL4::BindTexture(oldTexture); });

    // Generate mipmap
    glBindTexture(GL_TEXTURE_2D, textureObject->value);
    glGenerateMipmap(GL_TEXTURE_2D);
    POMDOG_CHECK_ERROR_GL4("glGenerateMipmap");
}

const Texture2DObjectGL4& Texture2DGL4::GetTextureHandle() const
{
    POMDOG_ASSERT(textureObject);
    return *textureObject;
}

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
