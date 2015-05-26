// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "ErrorChecker.hpp"
#include "OpenGLPrerequisites.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Logging/LogStream.hpp"
#include <sstream>

namespace Pomdog {
namespace Detail {
namespace GL4 {
namespace {

struct ErrorCodeTag;
using ErrorCodeGL4 = Tagged<GLenum, ErrorCodeTag>;

static std::string ToString(ErrorCodeGL4 const& errorCode)
{
    switch (errorCode.value) {
    case GL_NO_ERROR: return "GL_NO_ERROR";
    case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
#if defined(GL_VERSION_4_3) && !defined(GL_VERSION_3_2)
    case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
#endif
    }
    std::stringstream ss;
    ss << "ErrorCode '" << reinterpret_cast<void const*>(errorCode.value) << "'";
    return ss.str();
}

} // unnamed namespace
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
        Log::Stream("Pomdog.RenderSystem", LogLevel::Warning)
            << ">>> File " << filename << ", line " << line << ", in " << command << "\n"
            << "OpenGL Error: " << ToString(errorCode);

        if (lines == (maxLine - 1U))
        {
            Log::Warning("Pomdog.RenderSystem", "OpenGL Error: More...");
        }
        ++lines;
    }
}
//-----------------------------------------------------------------------
} // namespace GL4
} // namespace Detail
} // namespace Pomdog
