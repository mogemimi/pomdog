// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/logging/log.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <ios>
#include <sstream>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {
namespace {

[[nodiscard]] std::string
ToString(GLenum errorCode) noexcept
{
    switch (errorCode) {
    case GL_NO_ERROR:
        return "GL_NO_ERROR";
    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";
#if defined(GL_VERSION_4_3) && !defined(GL_VERSION_3_2)
    case GL_STACK_OVERFLOW:
        return "GL_STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW:
        return "GL_STACK_UNDERFLOW";
#endif
    }
    std::stringstream ss;
    ss << "ErrorCode '" << std::hex << errorCode << "'";
    return ss.str();
}

} // namespace

std::unique_ptr<Error>
GetLastError() noexcept
{
    const auto errorCode = glGetError();
    if (errorCode == GL_NO_ERROR) {
        return nullptr;
    }
    return errors::make(ToString(errorCode));
}

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
void CheckError(const char* command, const char* filename, int line)
{
    const auto errorCode = glGetError();
    if (errorCode == GL_NO_ERROR) {
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

        Log::Warning("Pomdog.Graphics", stream.str());

        if (lines == (maxLine - 1U)) {
            Log::Warning("Pomdog.Graphics", "OpenGL Error: More...");
        }
        ++lines;
    }
}
#endif

} // namespace pomdog::gpu::detail::gl4
