//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "ErrorChecker.hpp"
#include <sstream>
#include <Pomdog/Utility/detail/Tagged.hpp>
#include <Pomdog/Logging/Log.hpp>
#include <Pomdog/Logging/LogStream.hpp>
#include "OpenGLPrerequisites.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {
//-----------------------------------------------------------------------
namespace {

struct ErrorCodeTag;
using ErrorCodeGL4 = Tagged<GLenum, ErrorCodeTag>;

static std::string ToString(Pomdog::Details::RenderSystem::GL4::ErrorCodeGL4 const& errorCode)
{
	switch (errorCode.value)
	{
	case GL_NO_ERROR: return "GL_NO_ERROR";
	case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
	case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
	case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
#if defined(GL_VERSION_4_3) || !defined(GL_VERSION_3_2)
	case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
	case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
#endif
	}
	std::stringstream ss;
	ss << "ErrorCode '" << reinterpret_cast<void const*>(errorCode.value) << "'";
	return ss.str();
}

}// unnamed namespace
//-----------------------------------------------------------------------
void ErrorChecker::CheckError(char const* command, char const* filename, int line)
{
	ErrorCodeGL4 const errorCode{ glGetError() };
	
	if (GL_NO_ERROR == errorCode.value) {
		return;
	}

	constexpr std::size_t maxLine = 30;

	static std::size_t lines = 0;
	if (lines < maxLine)
	{
		Log::Stream(LoggingLevel::Critical)
			<< ">>> File " << filename << ", line " << line << ", in " << command << "\n"
			<< "OpenGL Error: " << ToString(errorCode);

		if (lines == (maxLine - 1U))
		{
			Log::LogMessage("OpenGL Error: More...", LoggingLevel::Critical);
		}
		++lines;
	}
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
