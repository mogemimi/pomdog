//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "SamplerStateGL4.hpp"
#include <algorithm>
#include <utility>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Graphics/SamplerDescription.hpp>
#include "ErrorChecker.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {
//-----------------------------------------------------------------------
namespace {

using TextureAddressModeGL4 = Tagged<GLenum, TextureAddressMode>;

static TextureAddressModeGL4 ToTextureAddressModeGL4(TextureAddressMode const& address)
{
	switch (address)
	{
	case TextureAddressMode::Clamp: return TextureAddressModeGL4{ GL_CLAMP_TO_EDGE };
	case TextureAddressMode::Mirror: return TextureAddressModeGL4{ GL_MIRRORED_REPEAT };
	case TextureAddressMode::Wrap: return TextureAddressModeGL4{ GL_REPEAT };
	}
#ifdef _MSC_VER
	return TextureAddressModeGL4{ GL_REPEAT };
#endif
}

}// unnamed namespace
//-----------------------------------------------------------------------
SamplerStateGL4::SamplerStateGL4(SamplerDescription const& description)
{
	samplerObject = ([](){
		SamplerObjectGL4 sampler;
		glGenSamplers(1, sampler.data());
		return std::move(sampler);
	})();
	
	#ifdef DEBUG
	ErrorChecker::CheckError("glGenSamplers", __FILE__, __LINE__);
	#endif
		
	glSamplerParameteri(samplerObject->value, GL_TEXTURE_WRAP_S, ToTextureAddressModeGL4(description.AddressU).value);
	glSamplerParameteri(samplerObject->value, GL_TEXTURE_WRAP_T, ToTextureAddressModeGL4(description.AddressV).value);
	glSamplerParameteri(samplerObject->value, GL_TEXTURE_WRAP_R, ToTextureAddressModeGL4(description.AddressW).value);

	#ifdef DEBUG
	ErrorChecker::CheckError("glSamplerParameteri", __FILE__, __LINE__);
	#endif

	switch (description.Filter)
	{
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
	case TextureFilter::Anisotropic:
		{
			///@todo Not implemented:
			glSamplerParameteri(samplerObject->value, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glSamplerParameteri(samplerObject->value, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			#ifdef DEBUG
			ErrorChecker::CheckError("glSamplerParameteri", __FILE__, __LINE__);
			#endif

			POMDOG_ASSERT(1 <= description.MaxAnisotropy && description.MaxAnisotropy <= 16);

			GLfloat deviceMaxAnisotropy = 1.0f;
			
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &deviceMaxAnisotropy);

			#ifdef DEBUG
			ErrorChecker::CheckError("glGetFloatv", __FILE__, __LINE__);
			#endif

			deviceMaxAnisotropy = std::min(deviceMaxAnisotropy, static_cast<GLfloat>(description.MaxAnisotropy));

			glSamplerParameterf(samplerObject->value, GL_TEXTURE_MAX_ANISOTROPY_EXT, deviceMaxAnisotropy);
			
			#ifdef DEBUG
			ErrorChecker::CheckError("glSamplerParameterf", __FILE__, __LINE__);
			#endif
		}
		break;
	}

	{
		// Note: GL_TEXTURE_MIN_LOD: The initial value is -1000.
		POMDOG_ASSERT(description.MinMipLevel < description.MaxMipLevel);
		GLint const minMipLevel = std::max(1, static_cast<GLint>(std::max(description.MinMipLevel, -1000.0f)));
		glSamplerParameteri(samplerObject->value, GL_TEXTURE_MIN_LOD, minMipLevel);
		
		#ifdef DEBUG
		ErrorChecker::CheckError("glSamplerParameteri", __FILE__, __LINE__);
		#endif
	}
	{
		POMDOG_ASSERT(description.MaxMipLevel != 0);
		GLint const maxMipLevel = std::max(1, static_cast<GLint>(std::min(description.MaxMipLevel, 1000.0f)));
		glSamplerParameteri(samplerObject->value, GL_TEXTURE_MAX_LOD, maxMipLevel);

		#ifdef DEBUG
		ErrorChecker::CheckError("glSamplerParameteri", __FILE__, __LINE__);
		#endif
	}
}
//-----------------------------------------------------------------------
SamplerStateGL4::~SamplerStateGL4()
{
	if (samplerObject) {
		glDeleteSamplers(1, samplerObject->data());
	}
}
//-----------------------------------------------------------------------
void SamplerStateGL4::Apply(std::size_t index)
{
	static_assert(GL_TEXTURE19 == (GL_TEXTURE0 + 19), "");
	POMDOG_ASSERT(index <= 19);

	// Bind sampler
	POMDOG_ASSERT(samplerObject);
	glBindSampler(static_cast<GLuint>(index), samplerObject->value);
	
	#ifdef DEBUG
	ErrorChecker::CheckError("glBindSampler", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
