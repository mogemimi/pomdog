// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Texture2DGL4.hpp"
#include "ErrorChecker.hpp"
#include "TypesafeHelperGL4.hpp"
#include "../RenderSystem/SurfaceFormatHelper.hpp"
#include "../Utility/ScopeGuard.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include <algorithm>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {
//-----------------------------------------------------------------------
namespace {

static GLenum ToInternalFormatGL4(SurfaceFormat format)
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
	};

#ifdef _MSC_VER
	// FUS RO DAH!
	return GL_R8;
#endif
}
//-----------------------------------------------------------------------
static GLenum ToFormatComponents(SurfaceFormat format)
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
		///@note Cannot find format
		return GL_RED;
	};

#ifdef _MSC_VER
	// FUS RO DAH!
	return GL_RED;
#endif
}
//-----------------------------------------------------------------------
static GLenum ToPixelFundamentalType(SurfaceFormat format)
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
		///@note Cannot find format
		return GL_UNSIGNED_BYTE;
	};

#ifdef _MSC_VER
	// FUS RO DAH!
	return GL_UNSIGNED_BYTE;
#endif
}
//-----------------------------------------------------------------------
template <typename T>
static GLenum ToTextureUnitIndexGL4(T index)
{
	static_assert(std::is_unsigned<T>::value, "T is unsigned type.");

	static_assert(GL_TEXTURE0  == (GL_TEXTURE0 + 0), "");
	static_assert(GL_TEXTURE1  == (GL_TEXTURE0 + 1), "");
	static_assert(GL_TEXTURE2  == (GL_TEXTURE0 + 2), "");
	static_assert(GL_TEXTURE3  == (GL_TEXTURE0 + 3), "");
	static_assert(GL_TEXTURE4  == (GL_TEXTURE0 + 4), "");
	static_assert(GL_TEXTURE5  == (GL_TEXTURE0 + 5), "");
	static_assert(GL_TEXTURE6  == (GL_TEXTURE0 + 6), "");
	static_assert(GL_TEXTURE7  == (GL_TEXTURE0 + 7), "");
	static_assert(GL_TEXTURE8  == (GL_TEXTURE0 + 8), "");
	static_assert(GL_TEXTURE9  == (GL_TEXTURE0 + 9), "");

	return static_cast<GLenum>(GL_TEXTURE0 + index);
}
//-----------------------------------------------------------------------
static GLsizei MipmapImageDataBytes(GLsizei pixelWidth, GLsizei pixelHeight, GLsizei bytesPerBlock)
{
	return pixelWidth * pixelHeight * bytesPerBlock;
}
//-----------------------------------------------------------------------
static void SetPixelDataTexture2DCompressedGL4(std::uint32_t pixelWidth, std::uint32_t pixelHeight,
	std::uint32_t levelCount, SurfaceFormat format, std::uint8_t const* pixelData)
{
	POMDOG_ASSERT(format == SurfaceFormat::BlockComp1_UNorm ||
		format == SurfaceFormat::BlockComp2_UNorm ||
		format == SurfaceFormat::BlockComp3_UNorm);

	auto const internalFormat = ToInternalFormatGL4(format);
	auto const bytesPerBlock = SurfaceFormatHelper::ToBytesPerBlock(format);
	std::size_t startOffset = 0;

	POMDOG_ASSERT(pixelWidth > 0);
	POMDOG_ASSERT(pixelHeight > 0);

	GLsizei mipMapPixelWidth = pixelWidth;
	GLsizei mipMapPixelHeight = pixelHeight;

	POMDOG_ASSERT(levelCount >= 1);
	for (GLint mipmapLevel = 0; mipmapLevel < static_cast<GLint>(levelCount); ++mipmapLevel)
	{
		POMDOG_ASSERT(mipMapPixelWidth > 0);
		POMDOG_ASSERT(mipMapPixelHeight > 0);

		GLsizei const strideBytesPerMipmap = MipmapImageDataBytes(mipMapPixelWidth, mipMapPixelHeight, bytesPerBlock);

		glCompressedTexSubImage2D(GL_TEXTURE_2D, mipmapLevel, 0, 0,
			mipMapPixelWidth, mipMapPixelHeight, internalFormat, strideBytesPerMipmap,
			pixelData + startOffset);

		#ifdef DEBUG
		ErrorChecker::CheckError("glCompressedTexSubImage2D", __FILE__, __LINE__);
		#endif

		startOffset += strideBytesPerMipmap;

		mipMapPixelWidth = std::max((mipMapPixelWidth >> 1), 1);
		mipMapPixelHeight = std::max((mipMapPixelHeight >> 1), 1);
	}
}
//-----------------------------------------------------------------------
static void SetPixelDataTexture2DGL4(std::uint32_t pixelWidth, std::uint32_t pixelHeight,
	std::uint32_t levelCount, SurfaceFormat format, std::uint8_t const* pixelData)
{
	POMDOG_ASSERT(pixelWidth > 0);
	POMDOG_ASSERT(pixelHeight > 0);
	POMDOG_ASSERT(levelCount >= 1);

	auto const formatComponents = ToFormatComponents(format);
	auto const pixelFundamentalType = ToPixelFundamentalType(format);
	auto const bytesPerBlock = SurfaceFormatHelper::ToBytesPerBlock(format);

	GLsizei mipMapPixelWidth = pixelWidth;
	GLsizei mipMapPixelHeight = pixelHeight;

	std::size_t startOffset = 0;

	for (GLint mipmapLevel = 0; mipmapLevel < static_cast<GLint>(levelCount); ++mipmapLevel)
	{
		GLsizei const strideBytesPerMipmap = MipmapImageDataBytes(mipMapPixelWidth, mipMapPixelHeight, bytesPerBlock);

		glTexSubImage2D(GL_TEXTURE_2D, mipmapLevel, 0, 0,
			mipMapPixelWidth, mipMapPixelHeight, formatComponents, pixelFundamentalType,
			pixelData + startOffset);

		#ifdef DEBUG
		ErrorChecker::CheckError("glTexSubImage2D", __FILE__, __LINE__);
		#endif

		startOffset += strideBytesPerMipmap;

		mipMapPixelWidth = std::max((mipMapPixelWidth >> 1), 1);
		mipMapPixelHeight = std::max((mipMapPixelHeight >> 1), 1);
	}
}

}// unnamed namespace
//-----------------------------------------------------------------------
template<>
struct TypesafeHelperGL4::OpenGLGetTraits<Texture2DObjectGL4> {
	constexpr static GLenum bufferObjectBinding = GL_TEXTURE_BINDING_2D;
	constexpr static GLenum textureObjectTarget = GL_TEXTURE_2D;
};
//-----------------------------------------------------------------------
Texture2DGL4::Texture2DGL4(std::int32_t pixelWidth, std::int32_t pixelHeight,
	std::uint32_t levelCount, SurfaceFormat format)
{
	// Create Texture2D
	textureObject = ([]{
		Texture2DObjectGL4 nativeTexture;
		glGenTextures(1, nativeTexture.Data());
		return std::move(nativeTexture);
	})();

	auto const prevTexture = TypesafeHelperGL4::Get<Texture2DObjectGL4>();
	ScopeGuard scope([&prevTexture]{
		TypesafeHelperGL4::BindTexture(prevTexture);
	});

	POMDOG_ASSERT(textureObject);
	TypesafeHelperGL4::BindTexture(*textureObject);

	#ifdef DEBUG
	ErrorChecker::CheckError("glBindTexture", __FILE__, __LINE__);
	#endif

	POMDOG_ASSERT(pixelWidth > 0);
	POMDOG_ASSERT(pixelHeight > 0);
	POMDOG_ASSERT(levelCount >= 1);
	POMDOG_ASSERT(levelCount <= std::numeric_limits<GLsizei>::max());

	glTexStorage2D(GL_TEXTURE_2D, levelCount, ToInternalFormatGL4(format), pixelWidth, pixelHeight);

	#ifdef DEBUG
	ErrorChecker::CheckError("glTexStorage2D", __FILE__, __LINE__);
	#endif

	// Set mipmap levels.
	POMDOG_ASSERT(levelCount >= 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, levelCount - 1);
}
//-----------------------------------------------------------------------
Texture2DGL4::~Texture2DGL4()
{
	if (textureObject) {
		glDeleteTextures(1, textureObject->Data());
	}
}
//-----------------------------------------------------------------------
void Texture2DGL4::SetData(std::int32_t pixelWidth, std::int32_t pixelHeight,
	std::uint32_t levelCount, SurfaceFormat format, void const* pixelData)
{
	POMDOG_ASSERT(pixelWidth > 0);
	POMDOG_ASSERT(pixelHeight > 0);
	POMDOG_ASSERT(levelCount >= 1);
	POMDOG_ASSERT(pixelData != nullptr);

	auto const oldTextureObject = TypesafeHelperGL4::Get<Texture2DObjectGL4>();
	ScopeGuard scope([&oldTextureObject]{
		TypesafeHelperGL4::BindTexture(oldTextureObject);
	});

	POMDOG_ASSERT(textureObject);
	TypesafeHelperGL4::BindTexture(*textureObject);

	#ifdef DEBUG
	ErrorChecker::CheckError("glBindTexture", __FILE__, __LINE__);
	#endif

	switch (format) {
	case SurfaceFormat::BlockComp1_UNorm:
	case SurfaceFormat::BlockComp2_UNorm:
	case SurfaceFormat::BlockComp3_UNorm:
		SetPixelDataTexture2DCompressedGL4(pixelWidth, pixelHeight, levelCount, format,
			reinterpret_cast<std::uint8_t const*>(pixelData));
		break;
	default:
		SetPixelDataTexture2DGL4(pixelWidth, pixelHeight, levelCount, format,
			reinterpret_cast<std::uint8_t const*>(pixelData));
		break;
	}
}
//-----------------------------------------------------------------------
void Texture2DGL4::Apply(std::uint32_t index)
{
	#if defined(DEBUG) && !defined(NDEBUG)
	{
		static std::uint32_t const MaxCombinedTextureImageUnits = ([]{
			GLint maxCombinedTextureImageUnits = 0;
			glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedTextureImageUnits);
			return maxCombinedTextureImageUnits;
		})();

		POMDOG_ASSERT(index < MaxCombinedTextureImageUnits);
	}
	#endif

	glActiveTexture(ToTextureUnitIndexGL4(index));

	#ifdef DEBUG
	ErrorChecker::CheckError("glActiveTexture", __FILE__, __LINE__);
	#endif

	// Bind texture
	POMDOG_ASSERT(textureObject);
	glBindTexture(GL_TEXTURE_2D, textureObject->value);

	#ifdef DEBUG
	ErrorChecker::CheckError("glBindTexture", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
void Texture2DGL4::GenerateMipmap()
{
	POMDOG_ASSERT(textureObject);

	auto const prevTexture = TypesafeHelperGL4::Get<Texture2DObjectGL4>();
	ScopeGuard scope([&prevTexture]{
		TypesafeHelperGL4::BindTexture(prevTexture);
	});

	// Generate mipmap
	glBindTexture(GL_TEXTURE_2D, textureObject->value);
	glGenerateMipmap(GL_TEXTURE_2D);

	#ifdef DEBUG
	ErrorChecker::CheckError("glGenerateMipmap", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
Texture2DObjectGL4 const& Texture2DGL4::NativeHandle() const
{
	POMDOG_ASSERT(textureObject);
	return textureObject.value();
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog
