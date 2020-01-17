// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog::Detail::SoundSystem::OpenAL {

class ErrorCheckerAL {
public:
    static void CheckError(const char* command, const char* filename, int line);
};

} // namespace Pomdog::Detail::SoundSystem::OpenAL
