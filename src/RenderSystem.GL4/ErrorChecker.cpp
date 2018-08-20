// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "ErrorChecker.hpp"
#include "OpenGLPrerequisites.hpp"
#include "../Utility/Tagged.hpp"
#include "Pomdog/Logging/Log.hpp"
#include <sstream>

namespace Pomdog {
namespace Detail {
namespace GL4 {
namespace {

struct ErrorCodeTag;
using ErrorCodeGL4 = Tagged<GLenum, ErrorCodeTag>;

std::string ToString(const ErrorCodeGL4& errorCode)
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
    ss << "ErrorCode '" << reinterpret_cast<const void*>(errorCode.value) << "'";
    return ss.str();
}

} // unnamed namespace

void ErrorChecker::CheckError(const char* command, const char* filename, int line)
{
    ErrorCodeGL4 const errorCode{ glGetError() };

    if (GL_NO_ERROR == errorCode.value) {
        return;
    }

    constexpr std::size_t maxLine = 30;

    static std::size_t lines = 0;
    if (lines < maxLine) {
        std::stringstream stream;
        stream << ">>> File " << filename
            << ", line " << line
            << ", in " << command << "\n"
            << "OpenGL Error: " << ToString(errorCode);

        Log::Warning("Pomdog.RenderSystem", stream.str());

        if (lines == (maxLine - 1U)) {
            Log::Warning("Pomdog.RenderSystem", "OpenGL Error: More...");
        }
        ++lines;
    }
}

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
