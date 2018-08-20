// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "ErrorCheckerAL.hpp"
#include "PrerequisitesOpenAL.hpp"
#include "../Utility/Tagged.hpp"
#include "Pomdog/Logging/Log.hpp"
#include <cstddef>
#include <sstream>

namespace Pomdog {
namespace Detail {
namespace SoundSystem {
namespace OpenAL {
namespace {

struct ErrorCodeTag;
using ErrorCodeAL = Tagged<ALenum, ErrorCodeTag>;

std::string ToString(const ErrorCodeAL& errorCode)
{
    switch (errorCode.value) {
    case AL_NO_ERROR: return "AL_NO_ERROR";
    case AL_INVALID_ENUM: return "AL_INVALID_ENUM";
    case AL_INVALID_VALUE: return "AL_INVALID_VALUE";
    case AL_INVALID_OPERATION: return "AL_INVALID_OPERATION";
    case AL_OUT_OF_MEMORY: return "AL_OUT_OF_MEMORY";
    }
    std::stringstream ss;
    ss << "ErrorCode '" << reinterpret_cast<const void*>(errorCode.value) << "'";
    return ss.str();
}

} // unnamed namespace

void ErrorCheckerAL::CheckError(const char* command, const char* filename, int line)
{
    ErrorCodeAL const errorCode{alGetError()};

    if (AL_NO_ERROR == errorCode.value) {
        return;
    }

    constexpr std::size_t maxLine = 30;

    static std::size_t lines = 0;
    if (lines < maxLine) {
        std::stringstream stream;
        stream << ">>> File " << filename
            << ", line " << line
            << ", in " << command << "\n"
            << "OpenAL Error: " << ToString(errorCode);

        Log::Warning("Pomdog.SoundSystem", stream.str());

        if (lines == (maxLine - 1U)) {
            Log::Warning("Pomdog.SoundSystem", "OpenAL Error: More...");
        }
        ++lines;
    }
}

} // namespace OpenAL
} // namespace SoundSystem
} // namespace Detail
} // namespace Pomdog
