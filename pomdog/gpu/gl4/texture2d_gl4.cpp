// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/texture2d_gl4.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/backends/surface_format_helper.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/gpu/gl4/typesafe_helper_gl4.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/scope_guard.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::detail::ScopeGuard;

namespace pomdog::gpu::detail::gl4 {
namespace {

[[nodiscard]] GLenum
ToInternalFormatGL4(PixelFormat format) noexcept
{
    switch (format) {
    case PixelFormat::Invalid:
        // NOTE: unknown format
        return GL_R8;
    case PixelFormat::BlockComp1_UNorm:
        return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    case PixelFormat::BlockComp2_UNorm:
        return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    case PixelFormat::BlockComp3_UNorm:
        return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    case PixelFormat::R8_UNorm:
        return GL_R8;
    case PixelFormat::R8G8_UNorm:
        return GL_RG8;
    case PixelFormat::R8G8B8A8_UNorm:
    case PixelFormat::B8G8R8A8_UNorm:
        return GL_RGBA8;
    case PixelFormat::R11G11B10_Float:
        return GL_R11F_G11F_B10F;
    case PixelFormat::R32G32B32A32_Float:
        return GL_RGBA32F;
    case PixelFormat::R10G10B10A2_UNorm:
        return GL_RGB10_A2;
    case PixelFormat::R16G16_Float:
        return GL_RG16;
    case PixelFormat::R16G16B16A16_Float:
        return GL_RGBA16;
    case PixelFormat::R32_Float:
        return GL_R32F;
    case PixelFormat::A8_UNorm:
        return GL_R8;
    case PixelFormat::Depth16:
        return GL_DEPTH_COMPONENT16;
    case PixelFormat::Depth24Stencil8:
        return GL_DEPTH24_STENCIL8;
    case PixelFormat::Depth32:
        return GL_DEPTH_COMPONENT32;
    case PixelFormat::Depth32_Float_Stencil8_Uint:
        return GL_DEPTH32F_STENCIL8;
    }
    POMDOG_UNREACHABLE("Unsupported surface format");
}

[[nodiscard]] GLenum
ToFormatComponents(PixelFormat format) noexcept
{
    switch (format) {
    case PixelFormat::Invalid:
        // NOTE: unknown format
        return GL_RED;
    case PixelFormat::R8G8B8A8_UNorm:
    case PixelFormat::R16G16B16A16_Float:
    case PixelFormat::R32G32B32A32_Float:
    case PixelFormat::R10G10B10A2_UNorm:
        return GL_RGBA;
    case PixelFormat::R11G11B10_Float:
        return GL_RGB;
    case PixelFormat::R8G8_UNorm:
    case PixelFormat::R16G16_Float:
        return GL_RG;
    case PixelFormat::R8_UNorm:
    case PixelFormat::R32_Float:
    case PixelFormat::A8_UNorm:
        return GL_RED;
    case PixelFormat::B8G8R8A8_UNorm:
        return GL_BGRA;
    case PixelFormat::Depth16:
        return GL_DEPTH_COMPONENT;
    case PixelFormat::Depth24Stencil8:
        return GL_DEPTH_COMPONENT;
    case PixelFormat::Depth32:
        return GL_DEPTH_COMPONENT;
    case PixelFormat::Depth32_Float_Stencil8_Uint:
        return GL_DEPTH_COMPONENT;
    case PixelFormat::BlockComp1_UNorm:
    case PixelFormat::BlockComp2_UNorm:
    case PixelFormat::BlockComp3_UNorm:
        // Cannot find format
        return GL_RED;
    }
    POMDOG_UNREACHABLE("Unsupported surface format");
}

[[nodiscard]] GLenum
ToPixelFundamentalType(PixelFormat format) noexcept
{
    switch (format) {
    case PixelFormat::Invalid:
        return GL_UNSIGNED_BYTE;
    case PixelFormat::A8_UNorm:
    case PixelFormat::R8_UNorm:
    case PixelFormat::R8G8_UNorm:
    case PixelFormat::R8G8B8A8_UNorm:
    case PixelFormat::B8G8R8A8_UNorm:
        return GL_UNSIGNED_BYTE;
    case PixelFormat::R11G11B10_Float:
    case PixelFormat::R32G32B32A32_Float:
    case PixelFormat::R16G16_Float:
    case PixelFormat::R16G16B16A16_Float:
    case PixelFormat::R32_Float:
        return GL_FLOAT;
    case PixelFormat::R10G10B10A2_UNorm:
        return GL_UNSIGNED_INT_10_10_10_2;
    case PixelFormat::Depth16:
        return GL_UNSIGNED_SHORT;
    case PixelFormat::Depth24Stencil8:
        return GL_UNSIGNED_INT;
    case PixelFormat::Depth32:
        return GL_FLOAT;
    case PixelFormat::Depth32_Float_Stencil8_Uint:
        return GL_FLOAT;
    case PixelFormat::BlockComp1_UNorm:
    case PixelFormat::BlockComp2_UNorm:
    case PixelFormat::BlockComp3_UNorm:
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
    PixelFormat format,
    const void* pixelData)
{
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 1);
    POMDOG_ASSERT(pixelData != nullptr);
    POMDOG_ASSERT(
        format == PixelFormat::BlockComp1_UNorm ||
        format == PixelFormat::BlockComp2_UNorm ||
        format == PixelFormat::BlockComp3_UNorm);

    auto const internalFormat = ToInternalFormatGL4(format);

    const auto blockSize = [&format]() -> GLint {
        switch (format) {
        case PixelFormat::BlockComp1_UNorm:
            return 8;
        case PixelFormat::BlockComp2_UNorm:
            return 16;
        case PixelFormat::BlockComp3_UNorm:
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
    PixelFormat format,
    const void* pixelData)
{
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 1);
    POMDOG_ASSERT(pixelData != nullptr);
    POMDOG_ASSERT(
        format != PixelFormat::BlockComp1_UNorm &&
        format != PixelFormat::BlockComp2_UNorm &&
        format != PixelFormat::BlockComp3_UNorm);

    auto const formatComponents = ToFormatComponents(format);
    auto const pixelFundamentalType = ToPixelFundamentalType(format);
    auto const bytesPerBlock = SurfaceFormatHelper::toBytesPerBlock(format);

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

std::unique_ptr<Error>
Texture2DGL4::initialize(
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    std::int32_t levelCountIn,
    PixelFormat formatIn) noexcept
{
    pixelWidth_ = pixelWidthIn;
    pixelHeight_ = pixelHeightIn;
    levelCount_ = levelCountIn;
    format_ = formatIn;

    POMDOG_ASSERT(pixelWidth_ > 0);
    POMDOG_ASSERT(pixelHeight_ > 0);
    POMDOG_ASSERT(levelCount_ >= 1);

    // Create Texture2D
    textureObject_ = ([] {
        Texture2DObjectGL4 nativeTexture;
        glGenTextures(1, nativeTexture.Data());
        return nativeTexture;
    })();

    auto const prevTexture = TypesafeHelperGL4::Get<Texture2DObjectGL4>();
    ScopeGuard scope([&prevTexture] {
        TypesafeHelperGL4::BindTexture(prevTexture);
    });

    POMDOG_ASSERT(textureObject_);
    TypesafeHelperGL4::BindTexture(*textureObject_);
    POMDOG_CHECK_ERROR_GL4("glBindTexture");

    POMDOG_ASSERT(pixelWidth_ > 0);
    POMDOG_ASSERT(pixelHeight_ > 0);
    POMDOG_ASSERT(levelCount_ >= 1);
    POMDOG_ASSERT(levelCount_ <= std::numeric_limits<GLsizei>::max());

    glTexStorage2D(
        GL_TEXTURE_2D,
        levelCount_,
        ToInternalFormatGL4(format_),
        pixelWidth_,
        pixelHeight_);
    POMDOG_CHECK_ERROR_GL4("glTexStorage2D");

    // Set mipmap levels
    POMDOG_ASSERT(levelCount_ >= 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, levelCount_ - 1);

    if (format_ == PixelFormat::A8_UNorm) {
        // NOTE: Emulate DXGI_FORMAT_A8_UNORM or MTLPixelFormatA8Unorm on OpenGL.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_GREEN);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);
    }

    return nullptr;
}

Texture2DGL4::~Texture2DGL4()
{
    if (textureObject_) {
        glDeleteTextures(1, textureObject_->Data());
        POMDOG_CHECK_ERROR_GL4("glDeleteTextures");
    }
}

std::int32_t Texture2DGL4::getWidth() const noexcept
{
    return pixelWidth_;
}

std::int32_t Texture2DGL4::getHeight() const noexcept
{
    return pixelHeight_;
}

std::int32_t Texture2DGL4::getLevelCount() const noexcept
{
    return levelCount_;
}

PixelFormat Texture2DGL4::getFormat() const noexcept
{
    return format_;
}

void Texture2DGL4::getData(void* result, [[maybe_unused]] std::size_t offsetInBytes, std::size_t sizeInBytes) const
{
    const auto oldTexture = TypesafeHelperGL4::Get<Texture2DObjectGL4>();
    ScopeGuard scope([&] { TypesafeHelperGL4::BindTexture(oldTexture); });

    POMDOG_ASSERT(textureObject_);
    TypesafeHelperGL4::BindTexture(*textureObject_);
    POMDOG_CHECK_ERROR_GL4("glBindTexture");

    auto const formatComponents = ToFormatComponents(format_);
    auto const pixelFundamentalType = ToPixelFundamentalType(format_);
    auto const bytesPerBlock = SurfaceFormatHelper::toBytesPerBlock(format_);

    // FIXME: Not implemented yet.
    POMDOG_ASSERT(sizeInBytes >= static_cast<std::size_t>(bytesPerBlock * pixelWidth_ * pixelHeight_));
    if (sizeInBytes < static_cast<std::size_t>(bytesPerBlock * pixelWidth_ * pixelHeight_)) {
        return;
    }

    glGetTexImage(
        GL_TEXTURE_2D,
        0,
        formatComponents,
        pixelFundamentalType,
        result);
}

void Texture2DGL4::setData(const void* pixelData)
{
    POMDOG_ASSERT(pixelWidth_ > 0);
    POMDOG_ASSERT(pixelHeight_ > 0);
    POMDOG_ASSERT(levelCount_ >= 1);
    POMDOG_ASSERT(pixelData != nullptr);

    const auto oldTexture = TypesafeHelperGL4::Get<Texture2DObjectGL4>();
    ScopeGuard scope([&] { TypesafeHelperGL4::BindTexture(oldTexture); });

    POMDOG_ASSERT(textureObject_);
    TypesafeHelperGL4::BindTexture(*textureObject_);
    POMDOG_CHECK_ERROR_GL4("glBindTexture");

    switch (format_) {
    case PixelFormat::BlockComp1_UNorm:
    case PixelFormat::BlockComp2_UNorm:
    case PixelFormat::BlockComp3_UNorm:
        SetPixelDataTexture2DCompressedGL4(
            pixelWidth_, pixelHeight_, levelCount_, format_, pixelData);
        break;
    default:
        SetPixelDataTexture2DGL4(
            pixelWidth_, pixelHeight_, levelCount_, format_, pixelData);
        break;
    }
}

void Texture2DGL4::generateMipmap()
{
    POMDOG_ASSERT(textureObject_);

    const auto oldTexture = TypesafeHelperGL4::Get<Texture2DObjectGL4>();
    ScopeGuard scope([&] { TypesafeHelperGL4::BindTexture(oldTexture); });

    // Generate mipmap
    glBindTexture(GL_TEXTURE_2D, textureObject_->value);
    glGenerateMipmap(GL_TEXTURE_2D);
    POMDOG_CHECK_ERROR_GL4("glGenerateMipmap");
}

Texture2DObjectGL4 Texture2DGL4::getTextureHandle() const noexcept
{
    POMDOG_ASSERT(textureObject_);
    return *textureObject_;
}

} // namespace pomdog::gpu::detail::gl4
