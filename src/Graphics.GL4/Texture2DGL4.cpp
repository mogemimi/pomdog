// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Texture2DGL4.hpp"
#include "ErrorChecker.hpp"
#include "TypesafeHelperGL4.hpp"
#include "../Basic/Unreachable.hpp"
#include "../Graphics.Backends/SurfaceFormatHelper.hpp"
#include "../Utility/ScopeGuard.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>

namespace Pomdog::Detail::GL4 {
namespace {

[[nodiscard]] GLenum
ToInternalFormatGL4(SurfaceFormat format) noexcept
{
    switch (format) {
    case SurfaceFormat::Invalid:
        // NOTE: unknown format
        return GL_R8;
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
    case SurfaceFormat::Depth16:
        return GL_DEPTH_COMPONENT16;
    case SurfaceFormat::Depth24Stencil8:
        return GL_DEPTH24_STENCIL8;
    case SurfaceFormat::Depth32:
        return GL_DEPTH_COMPONENT32;
    case SurfaceFormat::Depth32_Float_Stencil8_Uint:
        return GL_DEPTH32F_STENCIL8;
    }
    POMDOG_UNREACHABLE("Unsupported surface format");
}

[[nodiscard]] GLenum
ToFormatComponents(SurfaceFormat format) noexcept
{
    switch (format) {
    case SurfaceFormat::Invalid:
        // NOTE: unknown format
        return GL_RED;
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
    case SurfaceFormat::Depth16:
        return GL_DEPTH_COMPONENT;
    case SurfaceFormat::Depth24Stencil8:
        return GL_DEPTH_COMPONENT;
    case SurfaceFormat::Depth32:
        return GL_DEPTH_COMPONENT;
    case SurfaceFormat::Depth32_Float_Stencil8_Uint:
        return GL_DEPTH_COMPONENT;
    case SurfaceFormat::BlockComp1_UNorm:
    case SurfaceFormat::BlockComp2_UNorm:
    case SurfaceFormat::BlockComp3_UNorm:
        // Cannot find format
        return GL_RED;
    }
    POMDOG_UNREACHABLE("Unsupported surface format");
}

[[nodiscard]] GLenum
ToPixelFundamentalType(SurfaceFormat format) noexcept
{
    switch (format) {
    case SurfaceFormat::Invalid:
        return GL_UNSIGNED_BYTE;
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
    case SurfaceFormat::R10G10B10A2_UNorm:
        return GL_UNSIGNED_INT_10_10_10_2;
    case SurfaceFormat::Depth16:
        return GL_UNSIGNED_SHORT;
    case SurfaceFormat::Depth24Stencil8:
        return GL_UNSIGNED_INT;
    case SurfaceFormat::Depth32:
        return GL_FLOAT;
    case SurfaceFormat::Depth32_Float_Stencil8_Uint:
        return GL_FLOAT;
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

    const auto blockSize = [&format]() -> GLint {
        switch (format) {
        case SurfaceFormat::BlockComp1_UNorm:
            return 8;
        case SurfaceFormat::BlockComp2_UNorm:
            return 16;
        case SurfaceFormat::BlockComp3_UNorm:
            return 16;
        default:
            break;
        }
        return 8;
    }();

    std::size_t startOffset = 0;

    GLsizei mipmapWidth = pixelWidth;
    GLsizei mipmapHeight = pixelHeight;

    for (GLint mipmapLevel = 0; mipmapLevel < levelCount; ++mipmapLevel) {
        POMDOG_ASSERT(mipmapWidth > 0);
        POMDOG_ASSERT(mipmapHeight > 0);

        const GLsizei strideBytesPerMipmap = ((mipmapWidth + 3) / 4) * ((mipmapHeight + 3) / 4) * blockSize;

        glCompressedTexSubImage2D(
            GL_TEXTURE_2D,
            mipmapLevel,
            0,
            0,
            mipmapWidth,
            mipmapHeight,
            internalFormat,
            strideBytesPerMipmap,
            reinterpret_cast<const std::uint8_t*>(pixelData) + startOffset);
        POMDOG_CHECK_ERROR_GL4("glCompressedTexSubImage2D");

        startOffset += strideBytesPerMipmap;

        mipmapWidth = std::max((mipmapWidth >> 1), 1);
        mipmapHeight = std::max((mipmapHeight >> 1), 1);
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
        const GLsizei strideBytesPerMipmap = MipmapImageDataBytes(
            mipMapPixelWidth,
            mipMapPixelHeight,
            bytesPerBlock);

        glTexSubImage2D(
            GL_TEXTURE_2D,
            mipmapLevel,
            0,
            0,
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

} // namespace

template <>
struct TypesafeHelperGL4::Traits<Texture2DObjectGL4> {
    constexpr static GLenum BufferBinding = GL_TEXTURE_BINDING_2D;
    constexpr static GLenum TextureTarget = GL_TEXTURE_2D;
};

std::shared_ptr<Error>
Texture2DGL4::Initialize(
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    std::int32_t levelCountIn,
    SurfaceFormat formatIn) noexcept
{
    pixelWidth = pixelWidthIn;
    pixelHeight = pixelHeightIn;
    levelCount = levelCountIn;
    format = formatIn;

    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 1);

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

    return nullptr;
}

Texture2DGL4::~Texture2DGL4()
{
    if (textureObject) {
        glDeleteTextures(1, textureObject->Data());
        POMDOG_CHECK_ERROR_GL4("glDeleteTextures");
    }
}

std::int32_t Texture2DGL4::GetWidth() const noexcept
{
    return pixelWidth;
}

std::int32_t Texture2DGL4::GetHeight() const noexcept
{
    return pixelHeight;
}

std::int32_t Texture2DGL4::GetLevelCount() const noexcept
{
    return levelCount;
}

SurfaceFormat Texture2DGL4::GetFormat() const noexcept
{
    return format;
}

void Texture2DGL4::GetData(void* result, [[maybe_unused]] std::size_t offsetInBytes, std::size_t sizeInBytes) const
{
    const auto oldTexture = TypesafeHelperGL4::Get<Texture2DObjectGL4>();
    ScopeGuard scope([&] { TypesafeHelperGL4::BindTexture(oldTexture); });

    POMDOG_ASSERT(textureObject);
    TypesafeHelperGL4::BindTexture(*textureObject);
    POMDOG_CHECK_ERROR_GL4("glBindTexture");

    auto const formatComponents = ToFormatComponents(format);
    auto const pixelFundamentalType = ToPixelFundamentalType(format);
    auto const bytesPerBlock = SurfaceFormatHelper::ToBytesPerBlock(format);

    // FIXME: Not implemented yet.
    POMDOG_ASSERT(sizeInBytes >= static_cast<std::size_t>(bytesPerBlock * pixelWidth * pixelHeight));
    if (sizeInBytes < static_cast<std::size_t>(bytesPerBlock * pixelWidth * pixelHeight)) {
        return;
    }

    glGetTexImage(
        GL_TEXTURE_2D,
        0,
        formatComponents,
        pixelFundamentalType,
        result);
}

void Texture2DGL4::SetData(const void* pixelData)
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

Texture2DObjectGL4 Texture2DGL4::GetTextureHandle() const noexcept
{
    POMDOG_ASSERT(textureObject);
    return *textureObject;
}

} // namespace Pomdog::Detail::GL4
