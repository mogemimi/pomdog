// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "ErrorCheckerAL.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <sstream>

namespace Pomdog::Detail::OpenAL {
namespace {

[[nodiscard]] std::string
ToErrorCodeString(ALenum errorCode) noexcept
{
    switch (errorCode) {
    case AL_NO_ERROR:
        return "AL_NO_ERROR";
    case AL_INVALID_ENUM:
        return "AL_INVALID_ENUM";
    case AL_INVALID_VALUE:
        return "AL_INVALID_VALUE";
    case AL_INVALID_OPERATION:
        return "AL_INVALID_OPERATION";
    case AL_OUT_OF_MEMORY:
        return "AL_OUT_OF_MEMORY";
    }
    return std::to_string(errorCode);
}

} // namespace

[[nodiscard]] std::shared_ptr<Error>
MakeOpenALError(ALenum err, std::string&& message) noexcept
{
    message += ": ";
    message += ToErrorCodeString(err);
    return Errors::New(std::move(message));
}

#if defined(DEBUG) && !defined(NDEBUG)
void CheckError(const char* command, const char* filename, int line) noexcept
{
    const ALenum errorCode = alGetError();

    if (errorCode == AL_NO_ERROR) {
        return;
    }

    constexpr int maxLine = 30;

    static int lines = 0;
    if (lines < maxLine) {
        std::stringstream stream;
        stream << ">>> File " << filename
               << ", line " << line
               << ", in " << command << "\n"
               << "OpenAL Error: " << ToErrorCodeString(errorCode);

        Log::Warning("Pomdog.SoundSystem", stream.str());

        if (lines == (maxLine - 1)) {
            Log::Warning("Pomdog.SoundSystem", "OpenAL Error: More...");
        }
        ++lines;
    }
}
#endif

} // namespace Pomdog::Detail::OpenAL
